#include "SDLGUITexture.h"
#include "GUI.h"

#include "Managers/FrameMan.h"

#include "System/SDLHelper.h"

#include "SDL2_gfxPrimitives.h"

#include "GraphicalPrimitive.h"
#include "Box.h"

namespace RTE {
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SDLGUITexture::SDLGUITexture() {
		m_Width = g_FrameMan.GetPlayerScreenWidth();
		m_Height = g_FrameMan.GetPlayerScreenHeight();
		m_Texture = nullptr;
		m_ClipRect = glm::vec4(0);
		m_ColorKey = 0;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SDLGUITexture::SDLGUITexture(SharedTexture pTexture) {
		if (pTexture) {
			m_Texture = pTexture;
			m_Width = m_Texture->GetW();
			m_Height = m_Texture->GetH();
		} else {
			m_Texture = nullptr;
			m_Width = 0;
			m_Height = 0;
		}

		m_ClipRect = std::nullopt;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SDLGUITexture::SDLGUITexture(const SDLGUITexture &reference) {
		m_TextureFile = reference.m_TextureFile;
		m_Texture = reference.m_Texture;
		m_Width = reference.m_Width;
		m_Height = reference.m_Height;
		m_ClipRect = reference.m_ClipRect;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SDLGUITexture::SDLGUITexture(int width, int height, bool renderer) { Create(width, height, renderer); }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SDLGUITexture::~SDLGUITexture() = default; //{ Destroy(); }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SDLGUITexture::Create(const std::string filename) {
		m_TextureFile.SetDataPath(filename);
		m_Texture = m_TextureFile.GetAsTexture();
		m_Width = m_Texture->GetW();
		m_Height = m_Texture->GetH();
		m_Locked = false;
		return m_Texture.get();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SDLGUITexture::Create(int width, int height, bool renderer) {
		m_Width = width;
		m_Height = height;
		m_Texture = MakeTexture();
		m_Texture->Create(width, height, BitDepth::Indexed8, g_FrameMan.GetDefaultPalette());
		m_Texture->ClearColor();

		return 1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::Destroy() {
		if (m_Locked)
			Unlock();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::Lock() {
		if (!m_Locked && m_Texture) {
			m_Locked = true;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::Unlock() {
		if (m_Locked && m_Texture) {
			m_Locked = false;
			m_Texture->Update();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SDLGUITexture::Render(int x, int y, GUIRect *pRect, bool trans, std::optional<glm::vec4> clip) {
		RTEAssert(m_Texture.get(), "Tried drawing the screen onto itself.");
		// Texture uploads are fairly slow (when done en masse) so do it at the latest possible point.
		Unlock();
		glm::vec4 src;
		glm::vec4 dest{x, y, m_Width, m_Height};
		if (pRect) {
			src = {pRect->x, pRect->y, pRect->w, pRect->h};
			dest.z = pRect->w;
			dest.w = pRect->h;
			if (clip) {
				if (clip->x > dest.x) {
					dest.x = clip->x;
					dest.z -= clip->x - dest.x;
				}
				if (clip->x + clip->z < dest.x + dest.z) {
					dest.z = clip->x + clip->z - dest.x;
				}
				if (clip->y > dest.y) {
					dest.y = clip->y;
					dest.w -= clip->y - dest.y;
				}
				if (clip->y + clip->w < dest.y + dest.w) {
					dest.z = clip->y + clip->w - dest.y;
				}

				if (dest.z <= 0 || dest.w <= 0)
					return;
			}
		} else {
			src = {0, 0, m_Width, m_Height};
		}

		if (!trans)
			m_Texture->setBlendMode(BlendModes::None);
		std::cout << "guitexture render. w: " << m_Width << " h: " << m_Height << " tw: " << m_Texture->GetW() << " th: " << m_Texture->GetH() << " dw: " << dest.z << " dh: " << dest.w << std::endl;

		m_Texture->setColorMod(0, 255, 255);

		m_Texture->render(g_FrameMan.GetRenderer(), src, dest);
		if (!trans)
			m_Texture->setBlendMode(BlendModes::None);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SDLGUITexture::RenderScaled(int x, int y, int width, int height, bool trans) {
		// Texture uploads are fairly slow (when done en masse) so do it at the latest possible point.
		Unlock();
		glm::vec4 dest{x, y, width, height};
		if (!trans)
			m_Texture->setBlendMode(BlendModes::None);
		m_Texture->setColorMod(0, 255, 255);
		m_Texture->render(g_FrameMan.GetRenderer(), dest);
		if (!trans)
			m_Texture->setBlendMode(BlendModes::None);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::Blit(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect, bool trans) {
		RTEAssert(m_Texture.get(), "Tried drawing the screen to another bitmap.");
		SDLGUITexture *temp = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		RTEAssert(temp, "Invalid GUIBitmap passed to SDLGUITexture Draw!");

		if (!(temp->GetTexture())) {
			Render(x, y, pRect, trans, temp->m_ClipRect);
			return;
		}
		temp->Lock();
		Box src;
		Box dest({static_cast<float>(x), static_cast<float>(y)}, m_Width, m_Height);
		if (pRect) {
			src = {static_cast<float>(pRect->x), static_cast<float>(pRect->y), static_cast<float>(pRect->w), static_cast<float>(pRect->h)};
			if (m_ClipRect) {
				src = src.GetIntersection(*m_ClipRect);
			}
		} else {
			src = {0, 0, static_cast<float>(m_Width), static_cast<float>(m_Height)};
		}

		// SDL_Rect dest{x, y, 0, 0};
		m_Texture->blit(temp->GetTexture(), src, dest);
		// if (!trans)
		// 	SDL_SetSurfaceBlendMode(srcSurface.get(), SDL_BLENDMODE_NONE);

		// SDL_BlitSurface(srcSurface.get(), &src, destSurface.get(), &dest);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::BlitScaled(GUIBitmap *pDestBitmap, int x, int y, int width, int height, bool trans) {
		SDLGUITexture *temp = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		RTEAssert(temp, "GUIBitmap passed to SDLGUITexture Draw");

		if (!temp->GetTexture())
			RenderScaled(x, y, width, height, trans);

		temp->Lock();

		glm::vec4 dest{x, y, width, height};
		m_Texture->blit(temp->GetTexture(), dest);

		if (!trans)
			m_Texture->setBlendMode(BlendModes::None);

		m_Texture->blit(temp->GetTexture(), std::nullopt, dest);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::DrawLine(int x1, int y1, int x2, int y2, unsigned long color) {
		if (!m_Texture) {
			// lineColor(g_FrameMan.GetRenderer(), x1, y1, x2, y2, color);
		}

		Lock();

		if (x1 == x2)
			DrawRectangle(x1, y1, 1, y2, color, true);

		if (y1 == y2)
			DrawRectangle(x1, y1, x2, 1, color, true);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::DrawRectangle(int x, int y, int width, int height, unsigned long color, bool filled) {
		Lock();

		if (!m_Texture) {
			// if (filled) {
			// 	boxColor(g_FrameMan.GetRenderer(), x, y, x + width, y + height, color);
			// } else {
			// 	rectangleColor(g_FrameMan.GetRenderer(), x, y, x + width, y + height, color);
			// }
			return;
		}

		glm::vec4 rect{x, y, width, height};
		if (filled) {
			m_Texture->fillRect(rect, color);
		} else {
			m_Texture->rect(rect, color);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned long SDLGUITexture::GetPixel(int x, int y) const {
		return m_Texture->GetPixel(x, y);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::SetPixel(int x, int y, unsigned long color) {
		if (m_Texture)
			m_Texture->SetPixel(x, y, color);
		// else pixelRGBA(g_FrameMan.GetRenderer(), x, y, color >> 24, color >> 16, color >> 8, color);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SDLGUITexture::GetClipRect(GUIRect *rect) const {
		if (m_ClipRect && rect) {
			rect->x = m_ClipRect->x;
			rect->y = m_ClipRect->y;
			rect->w = m_ClipRect->z;
			rect->h = m_ClipRect->w;
			return;
		}
	}

	void SDLGUITexture::SetClipRect(GUIRect *rect) {
		if (!rect)
			return;

		m_ClipRect->x = rect->x;
		m_ClipRect->y = rect->y;
		m_ClipRect->z = rect->w;
		m_ClipRect->w = rect->h;
	}

	void SDLGUITexture::AddClipRect(GUIRect *rect) {
		if (!rect)
			return;
		if (!m_ClipRect) {
			m_ClipRect = glm::vec4(0);
			return;
		}
		Box tempRect(Vector(rect->x, rect->y), rect->w, rect->h);
		Box clipRect(*m_ClipRect);
		if (tempRect.IntersectsBox(clipRect))
			m_ClipRect = tempRect.GetIntersection(clipRect);
		else
			m_ClipRect = glm::vec4(0);
	}

} // namespace RTE
