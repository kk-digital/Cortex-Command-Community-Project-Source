#ifndef _RTESDLBITMAP_
#define _RTESDLBITMAP_

#include "GUI/Interface.h"
#include "System/ContentFile.h"
#include "System/SDLTexture.h"

struct GUIRect;
namespace RTE {
	class SDLGUITexture : public GUIBitmap {
	public:
		/// <summary>
		/// Constructor method to instantiate a SDLTexture
		/// </summary>
		SDLGUITexture();

		/// <summary>
		/// Constructor to instantiate a SDLTexture from an existing SDL_Texture
		/// </summary>
		/// <param name="pTexture">
		/// Pointer to a texture to instantiate from
		/// </param>
		/// <param name="needPixelAccess">
		/// Texture requests pixel access, this allows GetPixel and SetPixel to
		/// be called safely
		/// </param>
		SDLGUITexture(const std::shared_ptr<Texture> &pTexture);

		SDLGUITexture(const SDLGUITexture &reference);

		SDLGUITexture(int width, int height, bool renderer = false);

		/// <summary>
		/// Destructor method to clean up the SDLBitmap object
		/// </summary>
		~SDLGUITexture() override;

		/// <summary>
		/// Create a new texture from a fileName
		/// </summary>
		/// <param name="filename">File to create texture from</param>
		/// <returns> True if the Texture was successfully loaded</returns>
		bool Create(const std::string filename);

		/// <summary>
		/// Create an empty texture
		/// </summary>
		/// <param name="width">Width of the texture</param>
		/// <param name="height">Height of the texture</param>
		/// <param name="depth">
		/// Unused, all textures will be created to match the screen colordepth
		/// </param>
		/// <returns> True if the texture was successfully created </returns>
		bool Create(int width, int height, int) {Create(width, height, false);}

		bool Create(int width, int height, bool renderer);

		/// <summary>
		/// Destroys and resets the SDLTexture object.
		/// </summary>
		void Destroy() override;

		void Render(int x, int y, GUIRect *pRect, bool trans = true, GUIRect* clip = nullptr);
		void RenderScaled(int x, int y, int width, int height, bool trans = true);

		void Blit(GUIBitmap* pDestBitmap, int x, int y, GUIRect* pRect, bool trans = true);
		void BlitScaled(GUIBitmap* pDestBitmap, int x, int y, int width, int height, bool trans = true);

		/// <summary>
		/// Draw the SDLBitmap to the destination bitmap
		/// </summary>
		/// <param name="pDestBitmap">Pointer to the destination bitmap</param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</param>
		/// <param name="pRect">Destination size</param>
		void Draw(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) override { pDestBitmap ? Blit(pDestBitmap, x, y, pRect, false) : Render(x, y, pRect, false); }

		/// <summary>
		/// Draw the Bitmap with transparency (SDL doesn't distinguish this
		/// anymore)
		/// </summary>
		/// <param name="pDestBitmap">pointer to the Destination Bitmap</param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</parma>
		/// <param name="pRect">Destination size</param>
		void DrawTrans(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) override { pDestBitmap ? Blit(pDestBitmap, x, y, pRect) : Render(x, y, pRect); }

		/// <summary>
		/// Draw transparent Bitmap with Scaling
		/// </summary>
		/// <param name="pDestBitmap"></param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</param>
		/// <param name="width">width to scale to</param>
		/// <param name="height">height to scale to</param>
		void DrawTransScaled(GUIBitmap *pDestBitmap, int x, int y, int width, int height) override { pDestBitmap ? BlitScaled(pDestBitmap, x, y, width, height) : RenderScaled(x, y, width, height);}

		/// <summary>
		/// Draw a Line on the Bitmap
		/// </summary>
		/// <param name="x1">x coordinate of first point</param>
		/// <param name="y1">y coordinate of first point</param>
		/// <param name="x2">x coordinate of second point</param>
		/// <param name="y2">y coordinate of second point</param>
		void DrawLine(int x1, int y1, int x2, int y2, unsigned long color) override;

		/// <summary>
		/// Draw a rectangle in the bitmap
		/// </summary>
		/// <param name="x">x coordinate of top left corner</param>
		/// <param name="y">y coordinate of top left corne</param>
		/// <param name="width">width of rectangle</param>
		/// <param name="height">height of rectangle</param>
		/// <parma name="color">color of the rectangle
		void DrawRectangle(int x, int y, int width, int height, unsigned long color, bool filled) override;

		/// <summary>
		/// Get the color of the pixel at coordinates (x,y).
		/// For this the texture needs to be locked.
		/// </summary>
		/// <param name="x">x coordinate</param>
		/// <param name="y">y coordinate</param>
		/// <returns> Color of the Pixel at coordinate (x,y)</returns>
		unsigned long GetPixel(int x, int y) override;

		/// <summary>
		/// Set the Color of the pixel at coordinates (x,y).
		/// For this the texture needs to be locked.
		/// </summary>
		/// <param name="x">x coordinate</param>
		/// <param name="y">y coordinate</param>
		/// <param name="color">new color of the pixel</param>
		void SetPixel(int x, int y, unsigned long color) override;

		/// <summary>
		/// Get the width of the texture
		/// </summary>
		/// <returns> Width of the texture</returns>
		int GetWidth() override { return m_Width; }

		/// <summary>
		/// Get the height of the texture
		/// </summary>
		/// <returns> Height of the texture </returns>
		int GetHeight() override { return m_Height; }

		/// @deprecated
		/// <summary>
		/// Get the color depth of the texture
		/// </summary>
		int GetColorDepth() override { return 32; };

		/// <summary>
		/// Get the clipping rectangle
		/// </summary>
		/// <param name="rect">Pointer to a rectangle to store the data</param>
		void GetClipRect(GUIRect *rect) override;

		/// <summary>
		/// Set the clipping rectangle
		/// </summary>
		/// <param name="rect">The rectangle to clip to</param>
		void SetClipRect(GUIRect *rect) override;

		void ResetClipRect() override;

		/// <summary>
		/// Set the clipping rectangle as the intersection of the current
		/// clipping rectangle and the given rectangle
		/// </summary>
		/// <param name="rect"> pointer to the rectangle to intersect</param>
		void AddClipRect(GUIRect *rect) override;

		/// <summary>
		/// Get the path to the datafile object in use by this GUIBitmap
		/// </summary>
		/// <returns> The Path to the datafile </returns>
		std::string GetDataPath() override {
			return m_TextureFile.GetDataPath();
		}

		/// <summary>
		/// Get the texture of the SDLTexture object
		/// </summary>
		SharedTexture GetTexture() { return m_Texture; }

		/// <summary>
		/// Returns wether this SDLTexture has texture data.
		/// </summary>
		/// <returns> True if a texture is set</returns>
		bool HasBitmap() override { return m_Texture != 0; }

		void Lock();

		void Unlock();

		void SetColorKey(unsigned long key) override { m_ColorKey = key;}

	private:
		ContentFile m_TextureFile;

		uint32_t m_ColorKey;

		SharedTexture m_Texture;

		bool m_Locked;

		std::unique_ptr<SDL_Rect> m_ClipRect;
		int m_Width;
		int m_Height;

	public:

	};
} // namespace RTE
#endif
