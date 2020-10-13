#include "SDLFrameMan.h"
#include "SDLTexture.h"
#include "SDLScreen.h"
#include "Constants.h"

namespace RTE {
	const std::string FrameMan::c_ClassName = "FrameMan";
	void FrameMan::Clear() {
		m_Win = NULL;
		m_Renderer = NULL;

		m_NumScreens = SDL_GetNumVideoDisplays();
		SDL_GetDisplayBounds(0, &m_Resolution);

		m_ScreenResX = m_Resolution.w;
		m_ScreenResY = m_Resolution.h;

		m_ResX = 960;
		m_ResY = 540;
		m_ResMultiplier = 1;
		m_NewResX = m_ResX;
		m_NewResY = m_ResY;

		m_ResChanged = false;
		m_Fullscreen = false;
		m_UpscaledFullscreen = false;

		m_HSplit = false;
		m_VSplit = false;
		m_HSplitOverride = false;
		m_VSplitOverride = false;

		m_PaletteFile.Reset();
	}

	void FrameMan::ValidateResolution(unsigned short &resX, unsigned short &resY,
	                                  unsigned short &resMultiplier) {}

	int FrameMan::Create() {
		ValidateResolution(m_ResX, m_ResY, m_ResMultiplier);
		m_Window = SDL_CreateWindow(
		    c_WindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX, m_ResY,
		    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS);

		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

		return 0;
	}

	int FrameMan::ReadProperty(std::string propName, Reader &reader){
		if (propName == "ResolutionX"){
			reader >> m_ResX;
			m_NewResX = m_ResX;
		} else if (propName == "ResolutionY"){
			reader >> m_ResY;
			m_NewResY = m_ResY;
		} else if (propName == "ResolutionMultiplier"){
			reader >> m_ResMultiplier;
		} else if (propName == "Fullscreen"){
			reader >> m_Fullscreen;
		} else if (propName == "HSplitScreen") {
			reader >> m_HSplitOverride;
		} else if (propName == "VSplitScreen") {
			reader >> m_VSplitOverride;
		} else if (propName == "PalleteFile") {
			reader >> m_PaletteFile;
		} else {
			return Serializable::ReadProperty(propName, reader);
		}
		return 0;
	}

	int FrameMan::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewProperty("ResolutionX");
		writer << m_ResX;
		writer.NewProperty("ResolutionY");
		writer << m_ResY;
		writer.NewProperty("ResolutionMultiplier");
		writer << m_ResMultiplier;
		writer.NewProperty("HSplitScreen");
		writer << m_HSplitOverride;
		writer.NewProperty("VSplitScreen");
		writer << m_VSplitOverride;
		writer.NewProperty("PaletteFile");
		writer << m_PaletteFile;

		return 0;
	}

	void FrameMan::Destroy(){
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);

		delete m_GUIScreen;
		delete m_LargeFont;
		delete m_SmallFont;

		Clear();
	}

	void Update(){

	}

	void Draw(){

	}

	void Destroy(){

	}
} // namespace RTE
