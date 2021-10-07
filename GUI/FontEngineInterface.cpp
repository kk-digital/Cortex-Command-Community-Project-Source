#include "FontEngineInterface.h"
#include "BitmapFont.h"
#include "RendererInterface.h"
#include "RmlUi/Core.h"

namespace RTE{
	RmlFontEngineInterface::RmlFontEngineInterface(RmlRenderInterface *renderInterface) {
		m_RenderInterface = renderInterface;
	}
	RmlFontEngineInterface::~RmlFontEngineInterface() = default;

	bool RmlFontEngineInterface::LoadFontFace(const std::string &fileName, bool) {
		Rml::Log::Message(Rml::Log::LT_INFO, "Loaded font %s", fileName.c_str());
		std::string fontFamily = std::filesystem::path(fileName).stem();
		m_Fonts.emplace(fontFamily, BitmapFont(fileName, m_RenderInterface));
		return true;
	}

	Rml::FontFaceHandle RmlFontEngineInterface::GetFontFaceHandle(const std::string &family, Rml::Style::FontStyle, Rml::Style::FontWeight, int) {
		robin_hood::unordered_map<std::string, BitmapFont>::iterator font = m_Fonts.find(family);
		if(font == m_Fonts.end()){
			font = m_Fonts.find("FontSmall");
			if(font == m_Fonts.end()){
				return reinterpret_cast<Rml::FontFaceHandle>(nullptr);
			}
		}
		return reinterpret_cast<Rml::FontFaceHandle>(&(*font));
	}

	bool RmlFontEngineInterface::LoadFontFace(const Rml::byte *, int, const std::string &family, Rml::Style::FontStyle, Rml::Style::FontWeight, bool) {
		if (family == "rmlui-debugger-font")
			return true;
		return false;
	}

	int RmlFontEngineInterface::GetSize(Rml::FontFaceHandle handle) { return reinterpret_cast<BitmapFont*>(handle)->GetSize(); }

	int RmlFontEngineInterface::GetXHeight(Rml::FontFaceHandle handle) { return reinterpret_cast<BitmapFont*>(handle)->GetSize(); }

	int RmlFontEngineInterface::GetLineHeight(Rml::FontFaceHandle handle) { return reinterpret_cast<BitmapFont*>(handle)->GetSize(); }

	int RmlFontEngineInterface::GetStringWidth(Rml::FontFaceHandle handle, const std::string &string, Rml::Character) { return reinterpret_cast<BitmapFont*>(handle)->GetStringWidth(string); }

	int RmlFontEngineInterface::GenerateString(Rml::FontFaceHandle faceHandle, Rml::FontEffectsHandle, const std::string &string, const Rml::Vector2f &position, const Rml::Colourb &colour, Rml::GeometryList &geometry) { return reinterpret_cast<BitmapFont*>(faceHandle)->GenerateString(string, position, colour, geometry); }
}
