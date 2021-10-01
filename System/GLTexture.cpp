#include "GLTexture.h"
#include "GL/glew.h"
namespace RTE {
	GLTexture::GLTexture() {
		Clear();
	}

	void GLTexture::Clear() {
		m_TextureID = 0;
		m_BPP = 32;
		m_Format = 0;
		m_VBO = 0;
		m_Pixels.clear();
	}
}
