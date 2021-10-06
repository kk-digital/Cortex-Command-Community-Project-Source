#include "RendererInterface.h"
#include "RmlUi/Core.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include "System/ContentFile.h"

namespace RTE {

	RmlRenderInterface::RmlRenderInterface(SDL_Renderer *renderer, SDL_Window *window) :
	    m_Renderer{renderer}, m_Window{window} {
		m_LoadedTextures.clear();
		m_LoadedTextures.emplace_back(nullptr);
	}

	RmlRenderInterface::~RmlRenderInterface() = default;

	void RmlRenderInterface::RenderGeometry(Rml::Vertex *vertices, int num_vertices, int *indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f &translation) {
		// Disable SDL shaders
		glUseProgramObjectARB(0);
		glPushMatrix();
		glTranslatef(translation.x, translation.y, 0);

		std::vector<Rml::Vector2f> Positions(num_vertices);
		std::vector<Rml::Colourb> Colors(num_vertices);
		std::vector<Rml::Vector2f> TexCoords(num_vertices);
		float textureWidth = 0.0f;
		float textureHeight = 0.0f;

		SharedTexture sdlTexture = nullptr;

		if (texture) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			sdlTexture = m_LoadedTextures.at(reinterpret_cast<size_t>(texture));
			sdlTexture->glBind(textureWidth, textureHeight);
		}

		for (int i = 0; i < num_vertices; ++i) {
			Positions[i] = vertices[i].position;
			Colors[i] = vertices[i].colour;
			if (sdlTexture) {
				TexCoords[i].x = vertices[i].tex_coord.x * textureWidth;
				TexCoords[i].y = vertices[i].tex_coord.y * textureHeight;
			} else
				TexCoords[i] = vertices[i].tex_coord;
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &Positions[0]);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &Colors[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords[0]);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		if (sdlTexture) {
			sdlTexture->glUnbind();
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glColor4f(1.0, 1.0, 1.0, 1.0);
		glPopMatrix();
		glDisable(GL_BLEND);
		// Reset blendmode and draw point outside screen to reset renderer state.
		SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderDrawPoint(m_Renderer, -1, -1);
	}

	void RmlRenderInterface::EnableScissorRegion(bool enable) {
		if (enable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	void RmlRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
		int windowWidth, windowHeight;
		SDL_GetWindowSize(m_Window, &windowWidth, &windowHeight);
		glScissor(x, windowHeight - (y + height), width, height);
	}

	bool RmlRenderInterface::LoadTexture(Rml::TextureHandle &texture_handle, Rml::Vector2i &texture_dimensions, const std::string &source) {

		m_LoadedTextures.emplace_back(ContentFile(source.c_str()).GetAsTexture());

		if (!m_LoadedTextures.back())
			return false;

		texture_handle = m_LoadedTextures.size() - 1;

		texture_dimensions.x = m_LoadedTextures.back()->getW();
		texture_dimensions.y = m_LoadedTextures.back()->getH();

		return true;
	}

	bool RmlRenderInterface::GenerateTexture(Rml::TextureHandle &texture_handle, const Rml::byte *source, const Rml::Vector2i &source_dimensions) {
		texture_handle = reinterpret_cast<Rml::TextureHandle>(m_LoadedTextures.size());

		m_LoadedTextures.emplace_back(std::make_shared<Texture>(m_Renderer, source_dimensions.x, source_dimensions.y, SDL_PIXELFORMAT_RGBA32, source_dimensions.x * 4, source, SDL_TEXTUREACCESS_STATIC));

		return true;
	}

	void RmlRenderInterface::ReleaseTexture(Rml::TextureHandle) {}
} // namespace RTE
