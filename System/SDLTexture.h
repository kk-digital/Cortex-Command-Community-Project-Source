#ifndef _SDLSYSTEMTEXTURE_
#define _SDLSYSTEMTEXTURE_

// Forward declarations

struct SDL_Texture;
void SDL_DestroyTexture(SDL_Texture *p);

struct SDL_Renderer;
struct SDL_Point;
struct SDL_Rect;

namespace RTE {
	/// <summary>
	/// This is a wrapper class for SDL_Texture and is meant to be header safe
	/// and used with SDLHelper.h
	/// </summary>
	class Texture {
		friend class SceneLayer;
		friend class ContentFile;

	public:
		/// <summary>
		/// Create a texture object from an existing one.
		/// </summary>
		/// <param name="texture">
		/// The texture to be assigned. Since this is a move assignment, texture
		/// must be an r-value
		/// </param>
		Texture(Texture &&texture);

		/// <summary>
		/// Create a Texture from a file, with the given texture access
		/// </summary>
		/// <param name="filename">
		/// A string representing the path of the image file to load
		/// </param>
		/// <param name="access">
		/// One of <a href="https://wiki.libsdl.org/SDL_TextureAccess">
		/// SDL_TextureAccess</a>
		/// </param>
		Texture(std::string filename, int access = 0);

		~Texture() { Reset(); }

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
		/// SDL_GetError
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
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &dest);

		/// <summary>
		/// Render a portion of the texture to the screen.
		/// </summary>
		/// <param name="pRenderer">
		/// Render context to be rendered to.
		/// </param>
		/// <param name="src">
		/// SDL_Rect designating the area of the texture that will be drawn.
		/// </param>
		/// <param name="dest">
		/// SDL_Rect designating the area the texture will be drawn to. The
		/// texture will be stretched to fill the SDL_Rect.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, const SDL_Rect &src,
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
		/// SDL_GetError
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
		/// SDL_GetError
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
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
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
		/// One or more of <a
		/// href="https://wiki.libsdl.org/SDL_RendererFlip">SDL_RenderFlip</a>,
		/// multiple values may be bitwise or'd together.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
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
		/// SDL_GetError
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
		/// Lock the Texture for write access. This is only available for
		/// streaming access Textures.
		/// </summary>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int lock();

		/// <summary>
		/// Lock a region of the Texture. This is only available for streaming
		/// Textures
		/// </summary>
		/// <param name="region">
		/// An SDL_Rect designating the area to be locked.
		/// </param>
		int lock(const SDL_Rect &region);

		/// <summary>
		/// Unlock the Texture. (It is safe to call this on an unlocked texture)
		/// </summary>
		void unlock();

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
		/// A uint32_t in the pixel format of the texture
		/// </returns>
		uint32_t getPixel(int x, int y);

		/// <summary>
		/// Get the color of the pixel at (pos.x,pos.y). If pos is outside the Texture this returns 0xFFFFFF00, i.e. fully transparent white.
		/// </summary>
		/// <param name="pos">
		/// Position of the pixel to get.
		/// </param>
		/// <reutrns>
		/// A uint32_t in the pixel format of the texture
		/// </returns>
		uint32_t getPixel(SDL_Point pos);

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

	public:
		//! Width of the texture
		int w;
		//! Height of the texture
		int h;

	private:
		//! Deleter struct for unique_ptr
		struct sdl_texture_deleter {
			void operator()(SDL_Texture *p) { SDL_DestroyTexture(p); }
		};

		//! Internal SDL_Texture
		std::unique_ptr<SDL_Texture, sdl_texture_deleter> m_Texture;

		//! Texture Access specifier
		int m_Access;

		//! READ ONLY pixel array
		std::vector<uint32_t> m_PixelsRO;
		//! WRITE ONLY pixel array. Only accessible while Texture is locked
		void *m_PixelsWO;

		//! Size of one row of pixels in Memory, only meaningful while Texture
		//! is locked
		int m_Pitch;

	private:
		/// <summary>
		/// Clear the texture and reset member variables
		/// </summary>
		void Reset();

		/// <summary>
		/// Create an empty Texture. Only for use by friend classes
		/// </summary>
		Texture() { Reset(); }

		// No copying of textures allowed
		Texture(Texture &copy) = delete;
		Texture &operator=(const Texture &copy) = delete;

	public:
		//! Move assignment operator
		Texture &operator=(Texture &&texture);
	};
} // namespace RTE

#endif
