#include "SDLFrameMan.h"
#include "SDLTexture.h"
#include "SDLScreen.h"

namespace RTE {

	void FrameMan::Clear() {
		m_Win = NULL;
		m_Renderer = NULL;

		m_NumScreens = SDL_GetNumVideoDisplays();
		SDL_GetDisplayBounds(0, &m_Resolution);

		m_ScreenResX = m_Resolution.w;
		m_ScreenResY = m_Resoluion.h;

		m_ResX = 960;
		m_ResY = 540;
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
} // namespace RTE
