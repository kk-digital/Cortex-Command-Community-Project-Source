#include "GUI.h"
#include "SDLScreen.h"
#include "SDLGUITexture.h"

#include "Managers/FrameMan.h"

#include "System/SDLHelper.h"

namespace RTE {

	SDLScreen::SDLScreen() {
		m_GUIBitmap = std::make_shared<SDLGUITexture>(g_FrameMan.GetResX(), g_FrameMan.GetResY(), true);
	};

	void SDLScreen::Destroy() {}

	GUIBitmap *SDLScreen::CreateBitmap(const std::string &filename) {
		SDLGUITexture *pBitmap = new SDLGUITexture();
		if (!pBitmap)
			return nullptr;

		if (!pBitmap->Create(filename)) {
			delete pBitmap;
			return nullptr;
		}

		return pBitmap;
	}

	GUIBitmap *SDLScreen::CreateBitmap(int width, int height) {
		SDLGUITexture *pBitmap = new SDLGUITexture(width, height, false);

		if (!pBitmap)
			return nullptr;

		return pBitmap;
	}

	void SDLScreen::DrawBitmap(GUIBitmap *pGUIBitmap, int x, int y, GUIRect *pRect) {
		if (!pGUIBitmap)
			return;

		SDLGUITexture *pBitmap = dynamic_cast<SDLGUITexture *>(pGUIBitmap);
		if (!pBitmap)
			return;

		pBitmap->Draw(nullptr, x, y, pRect);
	}

	void SDLScreen::DrawBitmapTrans(GUIBitmap *pGUIBitmap, int x, int y, GUIRect *pRect) {
		if (!pGUIBitmap)
			return;

		SDLGUITexture *pBitmap = dynamic_cast<SDLGUITexture *>(pGUIBitmap);

		if (!pBitmap)
			return;

		pBitmap->DrawTrans(nullptr, x, y, pRect);
	}

	GUIBitmap *SDLScreen::GetBitmap() const {
		return m_GUIBitmap.get();
	}

	unsigned long SDLScreen::ConvertColor(unsigned long color, int targetDepth) {
		unsigned long returnColor{0};
		if (targetDepth == 8) {
			glm::vec4 col((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, (color >> 24) & 0xff);
			returnColor = g_FrameMan.GetDefaultPalette()->GetIndexFromColor(col / 255.0f);
		} else {
			glm::u8vec4 col = g_FrameMan.GetDefaultPalette()->at(color);
			returnColor = static_cast<uint32_t>(col.a) << 24 | static_cast<uint32_t>(col.r) << 16 | static_cast<uint32_t>(col.g) << 8 | static_cast<uint32_t>(col.b);
		}

		return returnColor;
	}

} // namespace RTE
