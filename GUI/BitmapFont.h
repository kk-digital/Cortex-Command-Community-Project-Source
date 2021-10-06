#ifndef _RTEBITMAPFONT_
#define _RTEBITMAPFONT_
#include "System/SDLTexture.h"
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Geometry.h>
#include <RmlUi/Core/Texture.h>

namespace RTE {
	class RmlRenderInterface;
	class BitmapFont {
	public:
		BitmapFont(const std::string &filename, RmlRenderInterface *renderInterface);
		~BitmapFont();
		int GenerateString(const std::string &string, const Rml::Vector2f &stringPosition, const Rml::Colourb &colour, Rml::GeometryList &geometryList);
		int GetSize();
		int GetXHeight();
		int GetLineHeight();
		int GetStringWidth(const std::string &string);

	private:
		Rml::Texture m_FontTexture; //!< The font texture.
		Rml::Vector2f m_TextureDimensions;

		int m_FontHeight;

		typedef struct {
			int m_Width;
			int m_Height;
			int m_Offset;
		} Character;

		std::array<Character, 256> m_Characters; //!< Texture offsets for each character.
		std::array<int, 256> m_Kerning; //!< The kerning for each character. //<- TODO

	private:
		void GenerateFont(RmlRenderInterface *renderInterface);
	};
} // namespace RTE
#endif
