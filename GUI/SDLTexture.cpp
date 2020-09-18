#include "SDLTexture.h"

namespace RTE {
	SDLBitmap::SDLBitmap() {
		m_BitmapFile.Reset();
		m_Texture = nullptr;
		m_SelfCreated = false;
	}

	SDLBitmap::SDLBitmap(SDL_Texture *pSurface) {
		m_BitmapFile.Reset();
		m_Texture = pSurface;
		m_SelfCreated = false;
	}

	bool SDLBitmap::Create(int width, int height, int depth) {
		m_SelfCreated = true;

		m_BitmapFile.Reset();
		m_Texture =
		    SDL_CreateTexture(g_FrameMan.GetRenderer(),
		                      SDL_GetWindowPixelFormat(g_FrameMan.GetWindow()),
		                      SDL_TEXTUREACCESS_TARGET, width, int height);
	}
} // namespace RTE
