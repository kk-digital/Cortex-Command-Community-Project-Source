#ifndef _RTE_GLTEXTURE_
#define _RTE_GLTEXTURE_
#include "GL/gl.h"
namespace RTE {
	class GLTexture {
	public:
		GLTexture();
		~GLTexture();

		void Clear();

		int getW();
		int getH();


	private:
		GLuint m_VBO;
		GLuint m_TextureID;
		int m_BPP;

		float m_Width;
		float m_Height;

		uint32_t m_Format;
		std::vector<GLubyte> m_Pixels;
	};
}
#endif
