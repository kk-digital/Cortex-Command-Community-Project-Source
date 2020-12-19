#ifndef _RTESDLBITMAP_
#define _RTESDLBITMAP_

#include "GUI/GUIRect.h"
#include "GUI/Interface.h"
#include "ContentFile.h"
#include <SDL2/SDL.h>

namespace RTE {
	class SDLTexture : public GUIBitmap {
	public:
		/// <summary>
		/// Constructor method to instantiate a SDLBitmap
		/// </summary>
		SDLTexture();

		/// <summary>
		/// Constructor to instantiate a SDLBitmap from an existing SDL_Texture
		/// </summary>
		/// <param name="pTexture">Pointer to a texture to instantiate from </param>
		SDLTexture(SDL_Texture *pSurface);

		/// <summary>
		/// Destructor method to clean up the SDLBitmap object
		/// </summary>
		~SDLTexture();

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
		bool Create(int width, int height, int depth = 8);

		/// <summary>
		/// Destroys and resets the SDLTextutre object.
		/// </summary>
		void Destroy() override;

		/// <summary>
		/// Draw the SDLBitmap to the destination bitmap
		/// </summary>
		/// <param name="pDestBitmap">Pointer to the destination bitmap</param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</param>
		/// <param name="pRect">Destination size</param>
		void Draw(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) override;

		/// <summary>
		/// Draw the Bitmap with transparency (SDL doesn't distinguish this anymore)
		/// </summary>
		/// <param name="pDestBitmap">pointer to the Destination Bitmap</param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</parma>
		/// <param name="pRect">Destination size</param>
		void DrawTrans(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) override;

		/// <summary>
		/// Draw transparent Bitmap with Scaling
		/// </summary>
		/// <param name="pDestBitmap"></param>
		/// <param name="x">x position on the target</param>
		/// <param name="y">y position on the target</param>
		/// <param name="width">width to scale to</param>
		/// <param name="height">height to scale to</param>
		void DrawTransScaled(GUIBitmap *pDestBitmap, int x, int y, int width,
		                     int height) override;

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
		void DrawRectangle(int x, int y, int width, int height, unsigned long color,
		                   bool filled) override;

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
		/// Lock the texture for direct pixel access
		/// </summary>
		/// <returns> true if the texture was locked successfully</returns>
		bool LockTexture() { return LockTexture(NULL); }

		/// <summary>
		/// Lock the texture in the specified rectangle
		/// </summary>
		/// <param name="rect"> Rectangle inside the texture to lock</param>
		bool LockTexture(SDL_Rect *rect);

		/// <summary>
		/// Unlock texture after editing pixels
		/// </summary>
		/// <returns> true if the texture was unlocked successfully</returns>
		bool UnlockTexture();

		/// <summary>
		/// Get the pointer to the pixels array
		/// </summary>
		void *GetPixels() { return m_Pixels; }

		/// <summary>
		/// Get the length of a row of pixels in memory
		/// </summary>
		/// <returns>integer representing length of a row</returns>
		int GetPitch() { return m_Pitch; }

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

		/// <summary>
		/// Get the color depth of the texture (deprecated)
		/// </summary>
		int GetColorDepth() override;

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

		/// <summary>
		/// Set the clipping rectangle as the intersection of the current clipping
		/// rectangle and the given rectangle
		/// </summary>
		/// <param name="rect"> pointer to the rectangle to intersect</param>
		void AddClipRect(GUIRect *rect) override;

		/// <summary>
		/// Get the path to the datafile object in use by this GUIBitmap
		/// </summary>
		/// <returns> The Path to the datafile </returns>
		std::string GetDataPath() override {return m_TextureFile.GetDataPath();}

		/// <summary>
		/// Get the texture of the SDLTexture object
		/// </summary>
		SDL_Texture *GetBitmap() {return m_Texture;}

		/// <summary>
		/// Returns wether this SDLTexture has texture data.
		/// </summary>
		/// <returns> True if a texture is set</returns>
		bool HasBitmap() override { return m_Texture != 0; }

	private:
		ContentFile m_TextureFile;

		SDL_Texture *m_Texture;
		bool m_SelfCreated;

		// Direct pixel access variables
		void *m_Pixels;
		int m_Pitch;

		SDL_Rect m_ClipRect;
		int m_Width;
		int m_Height;
	};
} // namespace RTE

#endif
