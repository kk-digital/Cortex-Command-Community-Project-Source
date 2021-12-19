#include "RenderTexture.h"
#include "GLTexture.h"
#include "GL/glew.h"
#include "GLCheck.h"

namespace RTE {
	RenderTexture::RenderTexture() {
		glCheck(glGenFramebuffers(1, &m_FBO));
		m_Texture = nullptr;
	}

	RenderTexture::~RenderTexture() {
		if (m_FBO) {
			glCheck(glDeleteFramebuffers(1, &m_FBO));
		}
	}

	RenderTexture::RenderTexture(std::shared_ptr<RenderTexture> ref) {
		glCheck(glGenFramebuffers(1, &m_FBO));
		SetTexture(ref->m_Texture);
	}
	RenderTexture::RenderTexture(std::shared_ptr<GLTexture> texture) {
		glCheck(glGenFramebuffers(1, &m_FBO));
		SetTexture(texture);
	}

	void RenderTexture::Create(int width, int height, BitDepth format) {
		m_Texture = MakeTexture();
		m_Texture->Create(width, height, format);

		assert(m_Texture->GetTextureID() != 0);

		glCheck(glBindTexture(GL_TEXTURE_2D, m_Texture->GetTextureID()));

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
		glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetTextureID(), 0));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR: Failed to initialize framebuffer" << std::endl;
		}

		glCheck(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		glCheck(glClear(GL_COLOR_BUFFER_BIT));

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void RenderTexture::Create(std::shared_ptr<GLTexture> texture) {
		m_Texture = texture;

		glCheck(glBindTexture(GL_TEXTURE_2D, texture->GetTextureID()));

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
		glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetTextureID(), 0));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR: Failed to initialize framebuffer" << std::endl;
		}

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void RenderTexture::Draw(RenderState &state) {
		state.m_FBO = m_FBO;
		RenderTarget::Draw(state);
	}

	void RenderTexture::DrawClear(glm::vec4 color) {
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

		glCheck(glClearColor(color.r, color.g, color.b, color.a));

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void RenderTexture::SetTexture(std::shared_ptr<GLTexture> texture) {
		Create(texture);
	}

	int RenderTexture::GetW() const {
		if (m_Texture)
			return m_Texture->GetW();
		return -1;
	}

	int RenderTexture::GetH() const {
		if (m_Texture)
			return m_Texture->GetH();

		return -1;
	}
} // namespace RTE
