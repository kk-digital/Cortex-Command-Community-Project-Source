#ifndef _SDLSYSTEMTEXTURE_
#define _SDLSYSTEMTEXTURE_

// Forward declarations
extern "C" {

struct SDL_Surface;
struct SDL_Texture;
struct SDL_PixelFormat;

struct SDL_Renderer;
struct SDL_Point;
typedef struct SDL_Rect SDL_Rect;
}
//! Deleter structs for unique_ptr
struct sdl_texture_deleter {
	void operator()(SDL_Texture *p);
};
struct sdl_format_deleter {
	void operator()(SDL_PixelFormat *p);
};

struct sdl_deleter;

namespace RTE {
	/// <summary>
	/// This is a wrapper class for SDL_Texture and the SDL_RenderCopy(Ex) functions
	/// and is meant to be used with SDLHelper.h.
	/// </summary>

	class Texture {
		friend class ContentFile;

	public:
		/// <summary>
		/// Create an empty Texture.
		/// </summary>
		Texture();

		/// <summary>
		/// Create a texture object from an existing one.
		/// </summary>
		/// <param name="texture">
		/// The texture to be assigned. Since this is a move assignment, texture
		/// must be an r-value.
		/// </param>
		Texture(Texture &&texture);

		/// <summary>
		/// Create an empty Texture with dimensions width x height.
		/// </summary>
		/// <param name="width">
		/// Width of the new Texture.
		/// </param>
		/// <param name="height">
		/// Height of the new Texture.
		/// </param>
		/// <param name="access">
		/// TextureAccess of the Texture defaults to render target.
		/// </param>
		Texture(SDL_Renderer *renderer, int width, int height, int access = 2);

		/// <summary>
		/// Copy constructor with rendering context.
		/// </summary>
		/// <param name="renderer">
		/// The Rendering context for which the texture is created.
		/// </param>
		/// <param name="texture">
		/// The Texture to be copied, this is a deep copy operation.
		/// </param>
		Texture(SDL_Renderer *renderer, const Texture &texture);

		virtual ~Texture();

		/// <summary>
		/// Render the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y);
		/// <summary>
		/// Render the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="dest">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest);

		/// <summary>
		/// Render the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="source">
		/// SDL_Rect designating the area of the Texture to be drawn.
		/// </param>
		/// <param name="dest">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &source,
		           const SDL_Rect &dest);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="pos">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y, double angle);

		/// <summary>
		/// Render the texture to the screen and flip horizontally or vertically
		/// according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="pos">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest, int flip);

		/// <summary>
		/// Render the texture to the screen and flip horizontally or vertically
		/// according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y, int flip);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">
		/// SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y, double angle,
		           int flip);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="center">
		/// The rotation pivot around which the destination rectangle will be rotated.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">
		/// SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y, double angle, const SDL_Point &center, int flip);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center,
		/// flipped horizontally or vertically according to flip and scaled.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">
		/// SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <param name="scale">
		/// Scalefactor
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, int x, int y, double angle,
		           int flip, double scale);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center,
		/// flipped horizontally or vertically according to flip and scaled.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="center">
		/// The pivot around which the destination rectangle is rotated.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">
		/// SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <param name="scale">
		/// Scalefactor
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer,
				   int x, int y,
				   double angle, const SDL_Point &center,
				   int flip, double scale);

		/// <summary>
		/// Render the texture to the screen rotated by angle around the center
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="pos">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="flip">
		/// One or more of <a href="https://wiki.libsdl.org/SDL_RendererFlip">
		/// SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle,
		           int flip);
		/// <summary>
		/// Render the texture to the screen rotated by angle around center
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="pos">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be drawn at.
		/// </param>
		/// <param name="center">
		/// The center of rotation
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle,
		           const SDL_Point &center, int flip);

		/// <summary>
		/// Render the silhouette of the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, int x, int y,
		                    uint32_t color);

		/// <summary>
		/// Render the silhouette of the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="source">
		/// A Rectangle inside the texture to be rendered
		/// </param>
		/// <param name="dest">
		/// The Rectangle describing the area that the texture will be rendered
		/// to, stretched to fit
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, const SDL_Rect &source,
		                    const SDL_Rect &dest, uint32_t color);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, int x, int y,
		                    uint32_t color, double angle);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, int x, int y,
		                    uint32_t color, double angle, int flip);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle,
		/// flipped horizontally or vertically according to flip and scaled.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <param name="scale">
		/// Scale to draw the Texture at.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, int x, int y,
		                    uint32_t color, double angle, int flip,
		                    double scale);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle,
		/// flipped horizontally or vertically according to flip and scaled.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="x">
		/// x position to draw the texture to.
		/// </param>
		/// <param name="y">
		/// y position to draw the texture to.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by
		/// </param>
		/// <param name="center">
		/// The pivot around which the destination rectangle is rotated
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <param name="scale">
		/// Scale to draw the Texture at.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, int x, int y,
		                    uint32_t color, double angle, const SDL_Point &center, int flip,
		                    double scale);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="source">
		/// The area of the Texture to draw.
		/// </param>
		/// <param name="dest">
		/// The area to draw to, the texture will be stretched as needed.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, const SDL_Rect &source,
		                    const SDL_Rect &dest, uint32_t color, double angle,
		                    int flip);

		/// <summary>
		/// Render the silhouette of the texture to the screen rotated by angle
		/// and flipped horizontally or vertically according to flip.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="source">
		/// The area of the Texture to draw.
		/// </param>
		/// <param name="dest">
		/// The area to draw to, the texture will be stretched as needed.
		/// </param>
		/// <param name="color">
		/// The color that the silhouette should be rendered.
		/// </param>
		/// <param name="angle">
		/// The angle the texture should be rotated by.
		/// </param>
		/// <param name="center">
		/// The rotation center that the destination rectangle will be rotated around.
		/// </param>
		/// <param name="flip">
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int renderFillColor(SDL_Renderer *renderer, const SDL_Rect &dest, uint32_t color, double angle, const SDL_Point &center, int flip);

		/// <summary>
		/// Lock the Texture for write access. This is only available for
		/// streaming access Textures.
		/// </summary>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError.
		/// </returns>
		int lock();

		bool isLocked() {return m_PixelsWO != nullptr && m_Pitch != 0;}

		/// <summary>
		/// Safely Lock a region of the Texture. This is only available for
		/// streaming Textures
		/// </summary>
		/// <param name="region">
		/// An SDL_Rect designating the area to be locked. This will be clipped
		/// should the region fall outside the texture
		/// </param>
		// int lock(const SDL_Rect &region);

		/// <summary>
		/// Unlock the Texture and update pixels in vram if neccessary.
		/// If a region of the Texture was locked, it is assumed that all pixels
		/// in that are were modified.
		/// (It is safe to call this on an unlocked
		/// texture)
		/// </summary>
		void unlock();

		/// <summary>
		/// Get the internal SDL_Texture. This will ONLY return non nullptr for
		/// render targets
		/// </summary>
		SDL_Texture *getAsRenderTarget() {
			if (m_Access == 2) {
				return m_Texture.get();
			}
			return nullptr;
		}

		/// <summary>
		/// Get the width of the Texture
		/// </summary>
		int getW() const { return w; }

		/// <summary>
		/// Get the width of the Texture
		/// </summary>
		int getH() const { return h; }

		/// <summary>
		/// Get the textureaccess of the underlying texture
		/// </summary>
		int getAccess() const { return m_Access; }

		/// <summary>
		/// Get the pixel format of the Texture
		/// </summary>
		/// <returns>
		/// An uint32_t representing one of SDL_PixelFormatEnum
		/// </returns>
		uint32_t getFormat() { return m_Format; }

		/// <summary>
		/// Get the color of the pixel at (x,y). If (x,y) is outside the texture
		/// this returns 0xFFFFFF00, i.e. fully transparent white.
		/// </summary>
		/// <param name="x">
		/// x position of the pixel to get.
		/// </param>
		/// <param name="y">
		/// y position of the pixel to get.
		/// </param>
		/// <reutrns>
		/// A uint32_t in the pixel format of the texture.
		/// </returns>
		uint32_t getPixel(int x, int y) const;

		/// <summary>
		/// Get the color of the pixel at (pos.x,pos.y). If pos is outside the
		/// Texture this returns 0xFFFFFF00, i.e. fully transparent white.
		/// </summary>
		/// <param name="pos">
		/// Position of the pixel to get.
		/// </param>
		/// <reutrns>
		/// A uint32_t in the pixel format of the texture.
		/// </returns>
		uint32_t getPixel(SDL_Point pos) const;

		/// <summary>
		/// Get the color of the pixel at (x,y) without bounds checking.
		/// </summary>
		/// <param name="x">
		/// x position of the pixel to get.
		/// </param>
		/// <param name="y">
		/// y position of the pixel to get.
		/// </param>
		/// <reutrns>
		/// A uint32_t in the pixel format of the texture.
		/// </returns>
		uint32_t getPixelLower(int x, int y) const;

		/// <summary>
		/// Get the color directly from the pixel array.
		/// </summary>
		/// <param name="index">
		/// Which item to  get.
		/// </param>
		/// <returns>
		/// A RGBA32 color.
		/// </returns>
		uint32_t getPixel(size_t index) const;

		/// <summary>
		/// Set a pixel at (x,y) to color.
		/// </summary>
		/// <param name="x">
		/// x coordinate of pixel to set.
		/// </param>
		/// <param name="y">
		/// y coordinate of pixel to set.
		/// </param>
		/// <param name="color">
		/// Color to set the pixel to in 0xAARRGGBB;
		/// </param>
		void setPixel(int x, int y, uint32_t color);

		/// <summary>
		/// Set a pixel at (x,y) to the color given by r,g,b and a.
		/// </summary>
		/// <param name="x">
		/// The x coordinate to set.
		/// </param>
		/// <param name="y">
		/// The y coordinate to set.
		/// </param>
		/// <param name="r">
		/// The red color component [0,255].
		/// </param>
		/// <param name="g">
		/// The green color component [0,255].
		/// </param>
		/// <param name="b">
		/// The blue color component [0,255].
		/// </param>
		/// <param name="a">
		/// The alpha value [0,255].
		/// </param>
		void setPixel(int x, int y, uint32_t r, uint32_t g, uint32_t b, uint32_t a);

		/// <summary>
		/// Set a pixel at (x,y) to color, without bounds checking.
		/// </summary>
		/// <param name="x">
		/// The x coordinate to set.
		/// </param>
		/// <param name="y">
		/// The y coordinate to set.
		/// </param>
		/// <param name="color">
		/// The color to set in 0xAARRGGBB.
		/// </param>
		void setPixelLower(int x, int y, uint32_t color);

		/// <summary>
		/// Set a region of the texture from a vector of pixels.
		/// </summary>
		/// <param name="setRect">
		/// The area to copy the pixels to.
		/// </param>
		/// <param name="pixels">
		/// The pixels to set. Should have pixel pitch of setRect.w.
		/// </param>
		void setPixels(const SDL_Rect &setRect, const std::vector<uint32_t> &pixels);

		/// <summary>
		/// Replaces all pixels of oldColor by newColor.
		/// </summary>
		/// <param name="oldColor">
		/// The color which should be replaced, formatted as 0xRRGGBBAA.
		/// </param>
		/// <param name="newColor">
		/// The color that will replace oldColor, formatted as 0xRRGGBBAA.
		/// </param>
		void remapColor(uint32_t oldColor, uint32_t newColor);

		/// <summary>
		/// Clear all pixels of the Texture to color
		/// </summary>
		/// <param name="color">
		/// The color to clear the Texture to in RGBA32. Defaults to transparent (0x00000000)
		/// </param>
		void clearAll(uint32_t color = 0);

		/// <summary>
		/// For a globally locked streamable texture fills the area designated by rect with color
		/// </summary>
		/// <param name="rect">
		/// An SDL_Rect that represents the rectangle to be filled
		/// </param>
		/// <param name="color">
		/// The color in RGBA32 to fill
		/// </param>
		void fillRect(const SDL_Rect &rect, uint32_t color);

		/// <summary>
		/// Fill the entire locked region with color
		/// </summary>
		/// <param name="color">
		/// The color in RGBA32 used to fill
		/// </param>
		void fillLocked(uint32_t color);

		/// <summary>
		/// For a locked streamable texture draw the outline of a rectangle in color.
		/// </summary>
		/// <param name="rect">
		/// The rectangle to draw.
		/// </param>
		/// <param name="color">
		/// The color the outline should have in RGBA32.
		/// </param>
		void rect(const SDL_Rect &rect, uint32_t color);

		/// <summary>
		/// Get the raw Pixels of the Texture for RW ops, offset by the locked
		/// area. If the Texture is locked modifying these will result in undefined behaviour
		/// </summary>
		uint32_t *getPixels();

		/// <summary>
		/// Get a read only vector of this texture's pixels.
		/// </summary>
		/// <returns>
		/// A const reference to the internal vector of pixels.
		/// </returns>
		const std::vector<uint32_t> &getPixelsRO() { return m_PixelsRO; }

		/// <summary>
		/// Get the writable pixels of this textures as a software surface.
		/// </summary>
		/// <returns>
		/// A unique_ptr of the SDL_Surface, which can be safely used with sdl blit functions
		/// </returns>
		std::unique_ptr<SDL_Surface, sdl_deleter> getPixelsAsSurface();

		/// <summary>
		/// Get the alpha value that will be multiplied onto all render
		/// operations
		/// </summary>
		/// <returns>
		/// The current alpha value
		/// </returns>
		int getAlphaMod();

		/// <summary>
		/// Set the alpha value that will be multiplied onto all render
		/// operations
		/// </summary>
		/// <param name="alpha">
		/// The alpha value to be used
		/// </param>
		int setAlphaMod(uint8_t alpha);

		/// <summary>
		/// Get the currently used blend mode for the Texture
		/// </summary>
		/// <returns>
		/// One of <a href="https://wiki.libsdl.org/SDL_BlendMode">
		/// SDL_BlendMode</a>
		/// </returns>
		int getBlendMode();

		/// <summary>
		/// Set the blend mode to be used in render operations
		/// </summary>
		/// <param name="blendMode">
		/// One of <a href="https://wiki.libsdl.org/SDL_BlendMode">
		/// SDL_BlendMode</a>
		/// </param>
		int setBlendMode(int blendMode);

		/// <summary>
		/// Get the current RGB modulation that is multiplied onto render.
		/// operations
		/// </summary>
		/// <returns>
		/// A uint32_t representing 0xRRGGBBFF.
		/// </returns>
		uint32_t getColorMod();

		/// <summary>
		/// Set values for red, green and blue to be multiplied onto render
		/// operations.
		/// </summary>
		/// <param name="r">
		/// The red value multiplier.
		/// </param>
		/// <param name="g">
		/// The green value multiplier.
		/// </param>
		/// <param name="b">
		/// The blue value multiplier.
		/// </param>
		int setColorMod(uint8_t r, uint8_t g, uint8_t b);

		/// <summary>
		/// Set values for all color channels to be multiplied onto render operations.
		/// </summary>
		/// <param name="rgb">
		/// The color multiplier.
		/// </param>
		int setColorMod(uint8_t rgb = 255) {return setColorMod(rgb, rgb, rgb);}

	private:
		//! Internal SDL_Texture
		std::unique_ptr<SDL_Texture, sdl_texture_deleter> m_Texture;

		//! Width of the texture
		int w;
		//! Height of the texture
		int h;

		//! Texture Access specifier
		int m_Access;

		//! Pixel Format specifier
		uint32_t m_Format;

		//! READ ONLY pixel array
		std::vector<uint32_t> m_PixelsRO;
		//! WRITE ONLY pixel array. Only accessible while Texture is locked
		void *m_PixelsWO;

		bool m_Updated; //!< Wether the texture needs to be updated on unlock

		//! Non NULL if locked with lock(SDL_Rect);
		std::unique_ptr<SDL_Rect> m_LockedRect;
		//! Size of one row of pixels in Memory, only meaningful while Texture
		//! is locked
		int m_Pitch;

		// static Texture used for rendering silhouettes, will be created as
		// needed, and must be able to contain the largest texture loaded
		std::unique_ptr<Texture> silhouetteTexture;

	private:
		/// <summary>
		/// Clear the texture and reset member variables
		/// </summary>
		void Reset();

		static uint32_t getNativeAlphaFormat(SDL_Renderer *renderer);

		void InitializeSilhouette(SDL_Renderer* renderer);

		Texture(const Texture &copy) = delete;
		Texture &operator=(const Texture &copy) = delete;

	public:
		//! Move assignment operator
		Texture &operator=(Texture &&texture);

		//! True if the Texture exists
		operator bool() const { return m_Texture.get(); }
	};

	typedef std::shared_ptr<Texture> SharedTexture;
	typedef std::unique_ptr<Texture> UniqueTexture;

} // namespace RTE

#endif
