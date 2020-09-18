#ifndef _RTESDLBITMAP_
#define _RTESDLBITMAP_

#include "GUI/Interface.h"
#include "ContentFile.h"
#include <SDL2/SDL.h>

namespace RTE {
	class SDLBitmap : public GUIBitmap {
  public:
		SDLBitmap();

		SDLBitmap(SDL_Texture *pSurface);

		~SDLBitmap();

		bool Create(const std::string filename);

		bool Create(int width, int height, int depth= 8);

		void Destroy() override;

		void Draw(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect);

		void DrawTrans(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect);

		void DrawTransScaled(GUIBitmap *pDestBitmap, int x, int y, int width, int height) override;
		void DrawLine(int x1, int y1, int x2, int y2, unsigned long color) override;

		void DrawRectangle(int x, int y, int width, int height, unsigned long color, bool filled) override;

		unsigned long GetPixel(int x, int y) override;

		void SetPixel(int x, int y, unsigned long color) override;

		int GetWidth() override;

		int GetHeight() override;

		int GetColorDepth() override;

		void GetClipRect(GUIRect *rect) override;

		void SetClipRect(GUIRect *pRect) override;

		void AddClipRect(GUIRect *rect) override;

		std::string GetDataPath() override;

		SDL_Texture *GetBitmap();

		bool HasBitmap() override { return m_pSurface != 0; }

	private:
		ContentFile m_BitmapFile;

		SDL_Texture *m_Texture;
		bool m_SelfCreated;
	}
} // namespace RTE

#endif
