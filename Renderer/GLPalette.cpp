#include "GLPalette.h"

namespace RTE{
	Palette::Palette(const std::array<GLubyte, PALETTESIZE * BYTESPERINDEX> &data, uint32_t format) : m_Palette(data), m_Format(format) {
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_1D, m_Texture);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, PALETTESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Palette.data());
		glBindTexture(GL_TEXTURE_1D, 0);
	}

	Palette::~Palette() {}
}
