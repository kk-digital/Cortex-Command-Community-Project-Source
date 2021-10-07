#include "GLTexture.h"

#include "Shader.h"

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
		m_VBO = 0;
		m_Pixels = nullptr;
	}
	void GLTexture::render(void *, float x, float y) {
		render(nullptr, {x, y});
	}
	void GLTexture::render(void *, glm::vec2 pos) {
		glBindTexture(GL_TEXTURE0, m_TextureID);

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), {pos, 0.0f});

		GetCurrentShader()->Use();

	}
	void GLTexture::render(void *, float x, float y, double angle) {}
	void GLTexture::render(void *, glm::vec2 pos, double angle) {}
	void GLTexture::render(void *, glm::vec2 pos, glm::vec2 scale) {}
	void GLTexture::render(void *, glm::vec2 pos, double angle, glm::vec2 scale) {}

	Shader *GLTexture::GetCurrentShader() {
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
