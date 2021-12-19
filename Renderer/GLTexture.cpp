#include "GLTexture.h"

#include "Shader.h"
#include "RenderState.h"
#include "RenderTarget.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "GLPalette.h"

#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"

#include <SDL2/SDL.h>
namespace RTE {

	void sdl_surface_deleter::operator()(SDL_Surface *p) { SDL_FreeSurface(p); }

	GLTexture::GLTexture() :
	    Surface() {
		Clear();
		glGenTextures(1, &m_TextureID);
	}

	GLTexture::GLTexture(std::shared_ptr<GLTexture> ref) :
	    Surface(*ref) {
		glGenTextures(1, &m_TextureID);
		m_ColorMod = ref->m_ColorMod;
		m_Shading = ref->m_Shading;
		m_ShaderBase = ref->m_ShaderBase;
		m_ShaderFill = ref->m_ShaderFill;
		m_ShaderCustom = ref->m_ShaderCustom;
	}

	GLTexture::~GLTexture() {
		if (m_TextureID) {
			glDeleteTextures(1, &m_TextureID);
		}
	}

	bool GLTexture::Create(int width, int height) {
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, m_BPP == 8 ? GL_RED : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	bool GLTexture::Create(int width, int height, BitDepth format, std::optional<std::shared_ptr<Palette>> palette) {

		m_Width = width;
		m_Height = height;
		m_BPP = format == BitDepth::BPP32 ? 32 : 8;
		if (Create(width, height)) {
			if (Surface::Create(width, height, format, palette)) {
				Update();
				return true;
			}
		}
		return false;
	}

	void GLTexture::Clear() {
		m_TextureID = 0;
		m_BPP = 32;

		m_ColorMod = glm::vec4(1.0f);
		m_BlendMode = BlendModes::Blend;

		m_Shading = Shading::Base;
		m_ShaderBase = nullptr;
		m_ShaderFill = nullptr;
		m_ShaderCustom = nullptr;
	}
	void GLTexture::render(RenderTarget *renderer, float x, float y) {
		render(renderer, {x, y});
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos) {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), {pos, 0.0f});

		RenderState render{shared_from_this(), translation, GetCurrentShader(), m_BlendMode, m_ColorMod};

		renderer->Draw(render);
	}
	void GLTexture::render(RenderTarget *renderer, float x, float y, float angle) {
		render(renderer, {x, y}, angle);
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, float angle) {
		glm::vec2 size = GetSize();
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::translate(modelTransform, {-0.5f * size, 0.0f});
		modelTransform = glm::scale(modelTransform, {size, 1.0f});
		RenderState render(shared_from_this(), modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, glm::vec2 scale) {
		glm::vec2 size = GetSize();
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * size, 1.0f});
		RenderState render(shared_from_this(), modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 scale) {
		glm::vec2 size = GetSize();
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::translate(modelTransform, {-0.5f * size, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * size, 1.0f});
		RenderState render(shared_from_this(), modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}

	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 center, glm::vec2 scale) {
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::translate(modelTransform, {center, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * GetSize(), 1.0f});
		RenderState render(shared_from_this(), modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}

	void GLTexture::render(RenderTarget *renderer, glm::vec4 src, glm::vec4 dest) {
		std::vector<Vertex> uvQuad;
		uvQuad.emplace_back(0.0f, 0.0f, src.x / m_Width, src.y / m_Height);
		uvQuad.emplace_back(0.0f, 1.0f, src.x / m_Width, (src.y + src.w) / m_Height);
		uvQuad.emplace_back(1.0f, 0.0f, (src.x + src.z) / m_Width, (src.y) / m_Height);
		uvQuad.emplace_back(1.0f, 1.0f, (src.x + src.z) / m_Width, (src.y + src.w) / m_Height);
		std::shared_ptr<VertexArray> uvVBO = std::make_shared<VertexArray>(std::move(uvQuad));

		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {dest.xy(), 0.0f});
		modelTransform = glm::scale(modelTransform, {dest.zw(), 1.0f});

		RenderState render(shared_from_this(), modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		render.m_Vertices = uvVBO;

		renderer->Draw(render);
	}

	void GLTexture::render(RenderTarget *renderer, glm::vec4 dest) {
		render(renderer, dest.xy(), {dest.z / GetW(), dest.w / GetH()});
	}

	void GLTexture::Update(std::optional<glm::vec4> region) {
		if (region) {}

		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_BPP == 8 ? GL_RED : GL_BGRA, GL_UNSIGNED_BYTE, GetPixels()->pixels);
	}

	void GLTexture::setColorMod(uint32_t rgb) {
		if (GetBitDepth() == 8) {
			float alphaMod = m_ColorMod.a;
			m_ColorMod = GetPalette()->at(rgb);
			m_ColorMod /= 255.0f;
			m_ColorMod.a = alphaMod;
		} else {
			m_ColorMod.r = ((rgb << 16) & 0xff) / 255.0f;
			m_ColorMod.g = ((rgb << 8) & 0xff) / 255.0f;
			m_ColorMod.b = ((rgb)&0xff) / 255.0f;
		}
	}

	void GLTexture::ClearColor(uint32_t color) {
		Surface::ClearColor(color);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glClearTexImage(GL_TEXTURE_2D, 0, m_BPP == 8 ? GL_RED : GL_BGRA, m_BPP == 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_INT_8_8_8_8, &color);
	}

	void GLTexture::Bind() {
		glBindTexture(GL_TEXTURE0, m_TextureID);
	}

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
