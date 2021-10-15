#include "GLTexture.h"

#include "Shader.h"
#include "RenderState.h"
#include "RenderTarget.h"

#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"

#include <SDL2/SDL.h>
namespace RTE {

	void sdl_surface_deleter::operator()(SDL_Surface *p) { SDL_FreeSurface(p); }

	GLTexture::GLTexture() {
		Clear();
	}

	void GLTexture::Clear() {
		m_TextureID = 0;
		m_BPP = 32;
		m_Format = 0;

		m_ColorMod = glm::vec4(1.0f);
		m_BlendMode = BlendModes::Blend;

		m_Shading = Shading::Base;
		m_ShaderBase = nullptr;
		m_ShaderFill = nullptr;
		m_ShaderCustom = nullptr;
	}
	void GLTexture::render(RenderTarget* renderer, float x, float y) {
		render(renderer, {x, y});
	}
	void GLTexture::render(RenderTarget* renderer, glm::vec2 pos) {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), {pos, 0.0f});

		RenderState render{this, translation, GetCurrentShader(), m_BlendMode, m_ColorMod};

		renderer->draw(render);

	}
	void GLTexture::render(RenderTarget * renderer, float x, float y, float angle) {
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {x, y, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::scale(modelTransform, {GetSize(), 1.0f});
	}
	void GLTexture::render(RenderTarget * renderer, glm::vec2 pos, double angle) {}
	void GLTexture::render(RenderTarget * renderer, glm::vec2 pos, glm::vec2 scale) {}
	void GLTexture::render(RenderTarget * renderer, glm::vec2 pos, double angle, glm::vec2 scale) {}

	std::shared_ptr<Shader> GLTexture::GetCurrentShader() {
		assert(m_ShaderBase);
		switch (m_Shading) {
			case Shading::Base:
				return m_ShaderBase;
				break;
			case Shading::Fill:
				return (m_ShaderFill ? m_ShaderFill : m_ShaderBase);
				break;
			case Shading::Custom:
				return (m_ShaderCustom ? m_ShaderCustom : m_ShaderBase);
				break;
		}
	}
} // namespace RTE
