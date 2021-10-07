#include "GL/glew.h"
namespace RTE {
	constexpr int BYTESPERINDEX = 4;
	constexpr int PALETTESIZE = 256;
	class Palette {
	public:
		Palette(const std::array<GLubyte, PALETTESIZE * BYTESPERINDEX> &data, uint32_t format);
		~Palette();
	private:
		GLuint m_Texture;

		std::array<GLubyte, PALETTESIZE * BYTESPERINDEX> m_Palette;
		uint32_t m_Format;
	};
}
