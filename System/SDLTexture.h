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

	public:
		// no empty texture allowed
		Texture() = delete;

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

		~Texture();

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
		/// <param name="pos">
		/// SDL_Point as position to draw the texture to.
		/// </param>
		/// <returns>
		/// 0 or a negative number which represents an error that can be read by
		/// SDL_GetError
		/// </returns>
		int render(SDL_Renderer *pRenderer, SDL_Point pos);

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
		int lock(const SDL_Rect &region);
		void unlock();

		int w;
		int h;

	private:
		//! Deleter struct for unique_ptr
		struct sdl_texture_deleter {
			void operator()(SDL_Texture *p) { SDL_DestroyTexture(p); }
		};

		//! Internal SDL_Texture
		std::unique_ptr<SDL_Texture, sdl_texture_deleter> m_Texture;

		//! READ ONLY pixel array
		std::vector<uint32_t> m_PixelsRO;
		//! WRITE ONLY pixel array. only accessible while Texture is locked
		void *m_PixelsWO;

		//! Size of one row of pixels in Memory, only useful while Texture is
		//! locked
		int m_Pitch;

		// Disable copy constructors
		Texture(Texture &copy) = delete;
		Texture &operator=(Texture &copy) = delete;
	};
} // namespace RTE

#endif
