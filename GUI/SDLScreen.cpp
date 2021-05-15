#include "GUI.h"
#include "SDLScreen.h"
#include "SDLGUITexture.h"

#include "Managers/FrameMan.h"

#include "System/SDLHelper.h"

namespace RTE{

	SDLScreen::SDLScreen() {
		m_GUIBitmap = std::make_shared<SDLGUITexture>(g_FrameMan.GetResX(), g_FrameMan.GetResY(), true);
	};

	void SDLScreen::Destroy() { }

	GUIBitmap* SDLScreen::CreateBitmap(const std::string filename){
		SDLGUITexture* pBitmap = new SDLGUITexture();
		if(!pBitmap)
			return nullptr;

		if(!pBitmap->Create(filename)){
			delete pBitmap;
			return nullptr;
		}

		return pBitmap;
	}

	GUIBitmap* SDLScreen::CreateBitmap(int width, int height) {
		SDLGUITexture* pBitmap = new SDLGUITexture(width, height, false);

		if(!pBitmap)
			return nullptr;

		return pBitmap;
	}

	void SDLScreen::DrawBitmap(GUIBitmap *pGUIBitmap, int x, int y, GUIRect *pRect){
		if(!pGUIBitmap)
			return;

		SDLGUITexture* pBitmap = dynamic_cast<SDLGUITexture*>(pGUIBitmap);
		if(!pBitmap)
			return;

		pBitmap->Draw(nullptr, x, y, pRect);
	}

	void SDLScreen::DrawBitmapTrans(GUIBitmap *pGUIBitmap, int x, int y, GUIRect *pRect){
		if(!pGUIBitmap)
			return;

		SDLGUITexture* pBitmap = dynamic_cast<SDLGUITexture*>(pGUIBitmap);

		if(!pBitmap)
			return;

		pBitmap->DrawTrans(nullptr, x, y, pRect);
	}

	GUIBitmap* SDLScreen::GetBitmap(){
		return m_GUIBitmap.get();
	}

	unsigned long SDLScreen::ConvertColor(unsigned long color, int){
		uint32_t returnColor{0};
		if(color>=0 && color <=255){
			returnColor = g_FrameMan.GetColorFromIndex(color);
		}else{
			returnColor = (color<<8)|0xFF;
		}

		return returnColor;
	}

}
