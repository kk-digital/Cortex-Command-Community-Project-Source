#include "SDLFrameMan.h"
#include "PostProcessMan.h"
#include "PrimitiveMan.h"
#include "PerformanceMan.h"
#include "ActivityMan.h"
#include "ConsoleMan.h"
#include "SettingsMan.h"
#include "UInputMan.h"

#include "Entities/SLTerrain.h"
#include "Entities/Scene.h"

#include "GUI/SDLTexture.h"
#include "GUI/SDLScreen.h"
#include "System/Constants.h"

#include "System/RTEError.h"

namespace RTE {
	const std::string FrameMan::c_ClassName = "FrameMan";

	void FrameMan::Clear() {
		m_Window = NULL;
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
		    c_WindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX,
		    m_ResY, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS);

		SetFullscreen(m_Fullscreen);

		RTEAssert(m_Window != NULL,
		          "Could not create Window because: " + SDL_GetError());

		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
		RTEAssert(m_Renderer != NULL,
		          "Could not create Renderer because: " + SDL_GetError());

		// Set integer scaling so we don't get artifacts by rendering subpixels.
		SDL_RenderSetIntegerScale(m_Renderer, SDL_TRUE);

		// Upscale the resolution to the set multiplier.
		SDL_RenderSetScale(m_Renderer, m_ResMultiplier, m_ResMultiplier);

		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_Renderer);

		return 0;
	}

	int FrameMan::ReadProperty(std::string propName, Reader &reader) {
		if (propName == "ResolutionX") {
			reader >> m_ResX;
			m_NewResX = m_ResX;
		} else if (propName == "ResolutionY") {
			reader >> m_ResY;
			m_NewResY = m_ResY;
		} else if (propName == "ResolutionMultiplier") {
			reader >> m_ResMultiplier;
		} else if (propName == "Fullscreen") {
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

	void FrameMan::Destroy() {
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);

		delete m_GUIScreen;
		delete m_LargeFont;
		delete m_SmallFont;

		Clear();
	}

	void FrameMan::Update() {
		g_PerformanceMan.Update();

		g_PrimitiveMan.ClearPrimitivesList();
	}

	void FrameMan::Draw() {
		int screenCount = 1;
		g_PostProcessMan.ClearScenePostEffects();

		list<PostEffect> screenRelativeEffects;
		list<Box> screenRelativeGlowBoxes;

		const Activity *pActivity = g_ActivityMan.GetActivity();

		for (int playerScreen = 0; playerScreen < screenCount; ++playerScreen) {
			screenRelativeEffects.clear();
			screenRelativeGlowBoxes.clear();

			SDLTexture drawScreen; // TODO: make player screens
		}

		// Reset the frame timer so we can measure how much time it takes until the
		// next frame is drawn
		g_PerformanceMan.ResetFrameTimer();
	}

	void FrameMan::SetFullscreen(bool fullscreen, bool endActivity) {
		SDL_SetWindowFullscreen(m_Window,
		                        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	}

} // namespace RTE
