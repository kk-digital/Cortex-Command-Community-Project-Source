#ifndef _RTEFONTENGINEINTERFACE_
#define _RTEFONTENGINEINTERFACE_
#include "RmlUi/Core/FontEngineInterface.h"

#include "BitmapFont.h"
namespace RTE {
	class RenderInterface;
	class FontEngineInterface: public Rml::FontEngineInterface {
	public:
		FontEngineInterface(RenderInterface *renderInterface);
		~FontEngineInterface() override;

		/// <summary> Called by RmlUi when it wants to load a font face from file. </summary>
		/// <param name="file_name"> The file to load the face from. </param>
		/// <return> True if the face was loaded successfully, false otherwise. </return>
		bool LoadFontFace(const std::string& fileName, bool fallbackFace) override;

		/// <summary> Called by RmlUi when it wants to load a font face from memory, registered using the provided family, style, and weight. </summary>
		/// <return> True if the face was loaded successfully, false otherwise. </return>
		bool LoadFontFace(const Rml::byte* data, int dataSize, const std::string& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, bool fallbackFace) override;

		/// <summary> Called by RmlUi when a font configuration is resolved for an element. Should return a handle that
		/// can later be used to resolve properties of the face, and generate string geometry to be rendered. </summary
		/// <param name="family"> The family of the desired font handle. </param>
		/// <param name="style"> The style of the desired font handle. </param>
		/// <param name="weight"> The weight of the desired font handle. </param>
		/// <param name="size"> The size of desired handle, in points. </param>
		/// <return> A valid handle if a matching (or closely matching) font face was found, NULL otherwise. </return>
		Rml::FontFaceHandle GetFontFaceHandle(const std::string& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, int size) override;

		/// <summary> Should return the point size of this font face. </summary>
		/// <param name="handle"> The font handle. </param>
		/// <return> The face's point size. </return>
		int GetSize(Rml::FontFaceHandle handle) override;

		/// <summary> Should return the pixel height of a lower-case x in this font face. </summary>
		/// <param name="handle"> The font handle. </param>
		/// <return> The height of a lower-case x. </return>
		int GetXHeight(Rml::FontFaceHandle handle) override;

		/// <summary> Should return the default height between this font face's baselines. </summary>
		/// <param name="handle"> The font handle. </param>
		/// <return> The default line height. </return>
		int GetLineHeight(Rml::FontFaceHandle handle) override;

		/// <summary> Called by RmlUi when it wants to retrieve the width of a string when rendered with this handle. </summary>
		/// <param name="handle"> The font handle. </param>
		/// <param name="string"> The string to measure. </param>
		/// <param name="priorCharacter"> The optionally-specified character that immediately precedes the string. This may have an impact on the string width due to kerning. </param>
		/// <return> The width, in pixels, this string will occupy if rendered with this handle. </return>
		int GetStringWidth(Rml::FontFaceHandle handle, const std::string& string, Rml::Character priorCharacter = Rml::Character::Null) override;

		/// <summary> Called by RmlUi when it wants to retrieve the geometry required to render a single line of text. </summary>
		/// <param name = "faceHandle"> The font handle. </param>
		/// <param name = "string"> The string to render. </param>
		/// <param name = "position"> The position of the baseline of the first character to render. </param>
		/// <param name = "colour"> The colour to render the text. </param>
		/// <param name = "geometry"> An array of geometries to generate the geometry into. </param>
		/// <return> The width, in pixels, of the string geometry. </return>
		int GenerateString(Rml::FontFaceHandle faceHandle, Rml::FontEffectsHandle fontEffectsHandle, const std::string& string, const Rml::Vector2f& position, const Rml::Colourb& colour, Rml::GeometryList& geometry) override;
	private:
		RenderInterface *m_RenderInterface; //!< The render interface stored for loading textures. Not owned.
		robin_hood::unordered_map<std::string, BitmapFont> m_Fonts; //!< Map of stored fonts.
	};
}
#endif
