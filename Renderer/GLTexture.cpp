#include "GLTexture.h"

#include "Shader.h"
#include "RenderState.h"
#include "RenderTarget.h"

#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"

#include <SDL2/SDL.h>
namespace RTE {

	void sdl_surface_deleter::operator()(SDL_Surface *p) { SDL_FreeSurface(p); }

	GLTexture::GLTexture() :Surface() {
		Clear();
		glGenTextures(1, &m_TextureID);
	}

	GLTexture::~GLTexture() {
		if(m_TextureID){
			glDeleteTextures(1, &m_TextureID);
		}
	}

	bool GLTexture::Create(int width, int height, uint32_t format) {
		(void)format;

		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
		m_Width = width;
		m_Height = height;
		m_BPP = 32;
		return true;
	}

	void GLTexture::Clear() {
		m_TextureID = 0;

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

		RenderState render{this, translation, GetCurrentShader(), m_BlendMode, m_ColorMod};

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
		RenderState render(this, modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, glm::vec2 scale) {
		glm::vec2 size = GetSize();
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * size, 1.0f});
		RenderState render(this, modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
	}
	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 scale) {
		glm::vec2 size = GetSize();
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::translate(modelTransform, {-0.5f * size, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * size, 1.0f});
		RenderState render(this, modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
	}

	void GLTexture::render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 center, glm::vec2 scale) {
		glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), {pos, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians(angle), {0.0f, 0.0f, 1.0f});
		modelTransform = glm::translate(modelTransform, {center, 0.0f});
		modelTransform = glm::scale(modelTransform, {scale * GetSize(), 1.0f});
		RenderState render(this, modelTransform, GetCurrentShader(), m_BlendMode, m_ColorMod);
		renderer->Draw(render);
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
