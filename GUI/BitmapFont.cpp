#include "BitmapFont.h"
#include <RmlUi/Core.h>
#include "RendererInterface.h"

namespace RTE {
	BitmapFont::BitmapFont(const std::string &filename, RenderInterface *renderInterface) {
		m_FontHeight = 0;
		m_Characters.fill(Character());
		m_Kerning.fill(0);

		Rml::Vector2i texDimensions;

		m_FontTexture.Set(filename);

		GenerateFont(renderInterface);
	}
	BitmapFont::~BitmapFont() = default;

	int BitmapFont::GenerateString(const std::string &string, const Rml::Vector2f &stringPosition, const Rml::Colourb &colour, Rml::GeometryList &geometryList) {
		int width = 0;
		geometryList.resize(1);
		Rml::Geometry &geometry = geometryList[0];

		geometry.SetTexture(&m_FontTexture);

		Rml::Vector<Rml::Vertex> &vertices = geometry.GetVertices();
		Rml::Vector<int> &indices = geometry.GetIndices();

		vertices.reserve(string.size() * 4);
		indices.reserve(string.size() * 6);

		Rml::Vector2f position = stringPosition.Round();

		for (std::string::const_iterator itChar = string.cbegin(); itChar < string.cend(); ++itChar) {
			int character = *itChar;
			if (character >= 256 || character < 32)
				continue;

			Character glyph = m_Characters.at(character);

			int charOffsetY = ((character - 32) / 16) * m_FontHeight;

			Rml::Vector2f UVTopLeft(glyph.m_Offset, charOffsetY);
			Rml::Vector2f UVBottomRight(glyph.m_Offset + glyph.m_Width, charOffsetY + m_FontHeight);

			Rml::GeometryUtilities::GenerateQuad(vertices.data() + (vertices.size() - 4), indices.data() + (indices.size() - 6), position, UVBottomRight - UVTopLeft, colour, UVTopLeft, UVBottomRight, vertices.size() - 4);

			width += glyph.m_Width;
			position.x += glyph.m_Width;
		}

		return width;
	}

	int BitmapFont::GetSize() { return m_FontHeight; }

	int BitmapFont::GetXHeight() { return m_Characters['x'].m_Height; }

	int BitmapFont::GetLineHeight() { return m_FontHeight; }

	int BitmapFont::GetStringWidth(const std::string &string) {
		int width = 0;

		for (std::string::const_iterator itCharacter = string.cbegin(); itCharacter < string.cend(); ++itCharacter) {
			int character = *itCharacter;

			if (character >= 256 || character < 32)
				continue;

			width += m_Characters[character].m_Width;
		}

		return width;
	}

	void BitmapFont::GenerateFont(RenderInterface *renderInterface) {

		SharedTexture fontTexture = renderInterface->GetTextureFromHandle(m_FontTexture.GetHandle(renderInterface));

		uint32_t separator = fontTexture->getPixel(0, 0);

		for (int y = 1; fontTexture->getH(); ++y) {
			if (fontTexture->getPixel(0, y) == separator) {
				m_FontHeight = y;
				break;
			}
		}

		int x = 0;
		int y = 0;
		int characterOnLine = 0;
		for (int chr = ' '; chr < 256; ++chr) {
			m_Characters[chr].m_Offset = x;

			int charWidth = 0;
			for (int pos = x; pos < fontTexture->getW(); ++pos, ++charWidth) {
				if (fontTexture->getPixel(pos, y) == separator) {
					break;
				}
			}

			m_Characters[chr].m_Width = charWidth - 1;

			int charHeight = 0;

			for (int j = y; j < y + m_FontHeight; ++j) {
				for (int i = x; i < x + charWidth; ++i) {
					uint32_t pixel = fontTexture->getPixel(i, j);
					if (pixel != separator && pixel != 0) {
						charHeight = std::max(charHeight, j - y);
					}
				}
			}

			m_Characters[chr].m_Height = charHeight;
			x += charWidth + 1;

			if (++characterOnLine >= 16) {
				characterOnLine = 0;
				x = 1;
				y += m_FontHeight;
				if ((y + m_FontHeight) > fontTexture->getH()) {
					break;
				}
			}
		}
	}
} // namespace RTE
