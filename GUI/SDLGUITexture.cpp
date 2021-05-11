#include "SDLGUITexture.h"
#include "GUI.h"

#include "Managers/FrameMan.h"

#include "System/SDLHelper.h"

namespace RTE {
	SDLGUITexture::SDLGUITexture() {}
	SDLGUITexture::SDLGUITexture(SharedTexture &pTexture) {
		m_Texture = pTexture;
		m_Width = m_Texture->getW();
		m_Height = m_Texture->getH();
		m_ClipRect = nullptr;
	}

	SDLGUITexture::SDLGUITexture(const SDLGUITexture &reference) {
		m_TextureFile = reference.m_TextureFile;
		m_Texture = reference.m_Texture;
		m_Width = reference.m_Width;
		m_Height = reference.m_Height;
		m_ClipRect = std::make_unique<SDL_Rect>(*reference.m_ClipRect);
	}

	SDLGUITexture::SDLGUITexture(int width, int height) { Create(width, height); }

	SDLGUITexture::~SDLGUITexture() = default;

	bool SDLGUITexture::Create(const std::string filename) {
		m_TextureFile.SetDataPath(filename);
		m_Texture = m_TextureFile.GetAsTexture();
		m_Width = m_Texture->getW();
		m_Height = m_Texture->getH();
		m_Locked = false;
		return m_Texture.get();
	}

	bool SDLGUITexture::Create(int width, int height, int) {
		m_Width = width;
		m_Height = height;
		m_Texture = std::make_shared<Texture>(g_FrameMan.GetRenderer(), width, height, SDL_TEXTUREACCESS_STREAMING);
		m_Locked = false;
		return m_Texture.get();
	}

	void SDLGUITexture::Destroy() {}

	void SDLGUITexture::Render(int x, int y, GUIRect *pRect, bool trans) {
		SDL_Rect src{pRect->x, pRect->y, pRect->w, pRect->h};
		SDL_Rect dest{x, y, pRect->w, pRect->h};

		if(!trans)
			m_Texture->setBlendMode(SDL_BLENDMODE_NONE);

		m_Texture->render(g_FrameMan.GetRenderer(), src, dest);
		if(!trans)
			m_Texture->setBlendMode(SDL_BLENDMODE_NONE);
	}

	void SDLGUITexture::RenderScaled(int x, int y, int width, int height, bool trans) {
		SDL_Rect dest{x, y, width, height};
		if(!trans)
			m_Texture->setBlendMode(SDL_BLENDMODE_NONE);
		m_Texture->render(g_FrameMan.GetRenderer(), dest);
		if(!trans)
			m_Texture->setBlendMode(SDL_BLENDMODE_BLEND);
	}

	void SDLGUITexture::Blit(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect, bool trans) {
		SDLGUITexture *temp = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		RTEAssert(temp, "GUIBitmap passed to SDLGUITexture Draw");

		temp->GetTexture()->lock();

		SDL_Rect src{pRect->x, pRect->y, pRect->w, pRect->h};
		SDL_Rect dest{x, y, 0, 0};
		std::unique_ptr<SDL_Surface, sdl_deleter> srcSurface{m_Texture->getPixelsAsSurface()};
		std::unique_ptr<SDL_Surface, sdl_deleter> destSurface{temp->GetTexture()->getPixelsAsSurface()};
		if(!trans)
			SDL_SetSurfaceBlendMode(srcSurface.get(), SDL_BLENDMODE_NONE);

		SDL_BlitSurface(srcSurface.get(), &src, destSurface.get(), &dest);
		temp->GetTexture()->unlock();
	}

	void SDLGUITexture::BlitScaled(GUIBitmap *pDestBitmap, int x, int y, int width, int height, bool trans) {
		SDLGUITexture *temp = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		RTEAssert(temp, "GUIBitmap passed to SDLGUITexture Draw");

		temp->GetTexture()->lock();

		SDL_Rect dest{x, y, width, height};
		std::unique_ptr<SDL_Surface, sdl_deleter> srcSurface{m_Texture->getPixelsAsSurface()};
		std::unique_ptr<SDL_Surface, sdl_deleter> destSurface{temp->GetTexture()->getPixelsAsSurface()};

		if(!trans)
			SDL_SetSurfaceBlendMode(srcSurface.get(), SDL_BLENDMODE_NONE);

		SDL_BlitScaled(srcSurface.get(), nullptr, destSurface.get(), &dest);
		temp->GetTexture()->unlock();
	}

	void SDLGUITexture::DrawLine(int x1, int y1, int x2, int y2, unsigned long color) {
		// TODO: bresenham
	}

	void SDLGUITexture::DrawRectangle(int x, int y, int width, int height, unsigned long color, bool filled) {
		if (!m_Locked)
			m_Texture->lock();

		SDL_Rect rect{x, y, width, height};
		if (filled) {
			m_Texture->fillRect(rect, color);
		} else {
			m_Texture->rect(rect, color);
		}
		if (!m_Locked)
			m_Texture->unlock();
	}

	unsigned long SDLGUITexture::GetPixel(int x, int y) {
		return m_Texture->getPixel(x, y);
	}

	void SDLGUITexture::SetPixel(int x, int y, unsigned long color) {
		m_Texture->setPixel(x, y, color);
	}

	void SDLGUITexture::GetClipRect(GUIRect *rect) {
		if (m_ClipRect && rect) {
			rect->x = m_ClipRect->x;
			rect->y = m_ClipRect->y;
			rect->w = m_ClipRect->w;
			rect->h = m_ClipRect->h;
			return;
		}
	}

	void SDLGUITexture::SetClipRect(GUIRect *rect) {
		if (!rect)
			return;

		m_ClipRect->x = rect->x;
		m_ClipRect->y = rect->y;
		m_ClipRect->w = rect->w;
		m_ClipRect->h = rect->h;
	}

	void SDLGUITexture::ResetClipRect() {
		m_ClipRect.reset();
	}

	void SDLGUITexture::AddClipRect(GUIRect *rect) {
		SDL_Rect intersectRect;
		SDL_Rect tempRect{rect->x, rect->y, rect->w, rect->h};
		if (SDL_IntersectRect(m_ClipRect.get(), &tempRect, &intersectRect))
			m_ClipRect = std::make_unique<SDL_Rect>(intersectRect);
		else
			m_ClipRect = std::make_unique<SDL_Rect>(SDL_Rect{0, 0, 0, 0});
	}

} // namespace RTE
