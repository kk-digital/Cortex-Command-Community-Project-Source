#include "SDLTexture.h"
#include "SDLHelper.h"
namespace RTE {
	std::unique_ptr<Texture> Texture::fillTarget;

	Texture::Texture(Texture &&texture) {
		texture.unlock();
		m_Texture = std::move(texture.m_Texture);
		w = texture.w;
		h = texture.h;
		m_Format = texture.m_Format;
		m_Rmask = texture.m_Rmask;
		m_Gmask = texture.m_Gmask;
		m_Bmask = texture.m_Bmask;
		m_Amask = texture.m_Amask;
		m_Access = texture.m_Access;
		m_PixelsRO = std::move(texture.m_PixelsRO);
		texture.Reset();
	}

	Texture::Texture(SDL_Renderer *renderer, const Texture &texture) {
		Reset();
		std::copy(texture.m_PixelsRO.begin(), texture.m_PixelsRO.end(),
		          m_PixelsRO);
		w = texture.w;
		h = texture.h;

		m_Format = getNativeAlphaFormat(renderer);
		setRGBAMasks();

		m_Texture.reset(SDL_CreateTexture(renderer, m_Format, texture.m_Access,
		                                  texture.w, texture.h));

		if (texture.m_Access == SDL_TEXTUREACCESS_STREAMING) {
			lock();
			SDL_ConvertPixels(w, h, texture.m_Format, m_PixelsRO.data(),
			                  w * sizeof(32), m_Format, m_PixelsWO, m_Pitch);
			unlock();
		} else {
			SDL_UpdateTexture(m_Texture.get(), nullptr, m_PixelsRO.data(),
			                  w * sizeof(uint32_t));
		}
	}
	Texture::Texture(SDL_Renderer *renderer, int width, int height,
	                 int access) :
	    w{width},
	    h{height}, m_Access{access} {
		m_Format = getNativeAlphaFormat(renderer);
		setRGBAMasks();
		m_PixelsWO = nullptr;
		m_Pitch = 0;
		m_PixelsRO.clear();
		m_PixelsRO.resize(width * height);
		m_Texture.reset(
		    SDL_CreateTexture(renderer, m_Format, access, width, height));
	}

	void Texture::Reset() {
		m_Texture.reset();
		w = 0;
		h = 0;
		m_Access = SDL_TEXTUREACCESS_STATIC;
		m_Format = SDL_PIXELFORMAT_UNKNOWN;
		m_Rmask = 0;
		m_Gmask = 0;
		m_Bmask = 0;
		m_Amask = 0;
		m_PixelsRO.clear();
		m_PixelsWO = nullptr;
		m_Pitch = 0;
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y) {
		return render(pRenderer, SDL_Rect{x, y, w, h});
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest) {
		return SDL_RenderCopy(pRenderer, m_Texture.get(), nullptr, &dest);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &source,
	                    const SDL_Rect &dest) {
		return SDL_RenderCopy(pRenderer, m_Texture.get(), &source, &dest);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                    double angle) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, nullptr, SDL_FLIP_NONE);
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y, double angle) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, angle);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                    int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest, 0,
		                        nullptr, static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y, double angle,
	                    int flip) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, angle, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                    double angle, int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, nullptr,
		                        static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                    double angle, const SDL_Point &center, int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, &center,
		                        static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y,
	                             uint32_t color) {
		return renderFillColor(renderer, SDL_Rect{0, 0, w, h},
		                       SDL_Rect{x, y, w, h}, color);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, const SDL_Rect &source,
	                             const SDL_Rect &dest, uint32_t color) {
		// Create a blend mode that draws all pixels of the Texture in the
		// targets color
		SDL_BlendMode blender{SDL_ComposeCustomBlendMode(
		    SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA,
		    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_SRC_ALPHA,
		    SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD)};

		// If neccessary extend the dimensions of the target to at least match
		// the size of this texture
		if (fillTarget->w < source.w && fillTarget->h < source.h)
			fillTarget.reset(new Texture(renderer, source.w, source.h));
		else if (fillTarget->w < source.w)
			fillTarget.reset(new Texture(renderer, source.w, fillTarget->h));
		else if (fillTarget->h < source.h)
			fillTarget.reset(new Texture(renderer, fillTarget->w, source.h));

		SDL_Rect dimensions{0, 0, source.w, source.h};

		// Store the active render target
		SDL_Texture *target{SDL_GetRenderTarget(renderer)};

		uint8_t r, g, b, a;
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

		// Set the render target to the fillTexture thatll be used to
		// generate the background color
		SDL_SetRenderTarget(renderer, fillTarget->m_Texture.get());
		SDL_SetRenderDrawColor(renderer, color & 0x00FF0000, color & 0x0000FF00,
		                       color & 0x000000FF, color & 0xFF000000);

		// Clear the target to the desired color
		SDL_RenderClear(renderer);

		// Set the texture blend mode so itll do the thing
		SDL_SetTextureBlendMode(m_Texture.get(), blender);

		// Copy the texture onto the intermediate target
		SDL_RenderCopy(renderer, m_Texture.get(), &source, &dimensions);

		// Reset the render target
		SDL_SetRenderTarget(renderer, target);

		// Reset the render draw color
		SDL_SetRenderDrawColor(renderer, r, g, b, a);

		// Render the texture area from the intermediate target to the screen
		return SDL_RenderCopy(renderer, fillTarget->m_Texture.get(),
		                      &dimensions, &dest);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y,
	                             uint32_t color, double angle) {
		return renderFillColor(renderer, SDL_Rect{0, 0, w, h},
		                       SDL_Rect{x, y, w, h}, color, angle,
		                       SDL_FLIP_NONE);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y,
	                             uint32_t color, double angle, int flip) {
		return renderFillColor(renderer, SDL_Rect{0, 0, w, h},
		                       SDL_Rect{x, y, w, h}, color, angle, flip);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, const SDL_Rect &source,
	                             const SDL_Rect &dest, uint32_t color,
	                             double angle, int flip) {
		// Create a custom blend mode that will draw all pixels of the texture
		// in the color of the target
		SDL_BlendMode blender{SDL_ComposeCustomBlendMode(
		    SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA,
		    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_SRC_ALPHA,
		    SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD)};

		// If neccessary extend the dimensions of the target to at least match
		// the size of this texture
		if (fillTarget->w < source.w && fillTarget->h < source.h)
			fillTarget.reset(new Texture(renderer, source.w, source.h));
		else if (fillTarget->w < source.w)
			fillTarget.reset(new Texture(renderer, source.w, fillTarget->h));
		else if (fillTarget->h < source.h)
			fillTarget.reset(new Texture(renderer, fillTarget->w, source.h));

		SDL_Rect dimensions{0, 0, source.w, source.h};

		// Store the active render target
		SDL_Texture *target{SDL_GetRenderTarget(renderer)};

		uint8_t r, g, b, a;
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

		// Set the render target to the fillTexture thatll be used to
		// generate the background color
		SDL_SetRenderTarget(renderer, fillTarget->m_Texture.get());
		SDL_SetRenderDrawColor(renderer, color & m_Rmask, color & m_Gmask,
		                       color & m_Bmask, color & m_Amask);

		// Clear the target to the desired color
		SDL_RenderClear(renderer);

		// Set the texture blend mode so itll do the thing
		SDL_SetTextureBlendMode(m_Texture.get(), blender);

		// Copy the texture onto the intermediate target
		SDL_RenderCopy(renderer, m_Texture.get(), &source, &dimensions);

		// Reset the render target
		SDL_SetRenderTarget(renderer, target);

		// Reset the render draw color
		SDL_SetRenderDrawColor(renderer, r, g, b, a);

		// Render the texture area from the intermediate target to the screen
		return SDL_RenderCopyEx(renderer, fillTarget->m_Texture.get(),
		                        &dimensions, &dest, angle, nullptr,
		                        static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::lock() {
		return SDL_LockTexture(m_Texture.get(), nullptr, &m_PixelsWO, &m_Pitch);
	}

	int Texture::lock(const SDL_Rect &region) {
		if (m_Access == SDL_TEXTUREACCESS_STREAMING) {
			m_LockedRect.reset(new SDL_Rect{region.x, region.y, 0, 0});

			if ((region.x + region.w) < w)
				m_LockedRect->w = region.w;
			else
				m_LockedRect->w = w - region.x;
			if ((region.y + region.h) < h)
				m_LockedRect->h = region.h;
			else
				m_LockedRect->h = h - region.y;

			return SDL_LockTexture(m_Texture.get(), m_LockedRect.get(),
			                       &m_PixelsWO, &m_Pitch);
		}
		return -1;
	}

	void Texture::unlock() {
		if (m_LockedRect)
			SDL_UpdateTexture(m_Texture.get(), m_LockedRect.get(),
			                  getPixelsRW(), w * sizeof(uint32_t));
		SDL_UnlockTexture(m_Texture.get());
		m_PixelsWO = nullptr;
		m_Pitch = 0;
	}

	uint32_t Texture::getPixel(int x, int y) {
		if (!m_PixelsRO.empty() && x < w && y < h)
			return m_PixelsRO[y * w + x];

		return m_Rmask|m_Gmask|m_Bmask;
	}

	uint32_t Texture::getPixel(SDL_Point pos) { return getPixel(pos.x, pos.y); }

	void Texture::setPixel(int x, int y, uint32_t color) {
		if (m_PixelsWO && x < w && y < h) {
			static_cast<uint32_t *>(m_PixelsWO)[x + y * w] = color;
			m_PixelsRO[x + y * w] = color;
		}
	}

	int Texture::setAlphaMod(uint8_t alpha) {
		return SDL_SetTextureAlphaMod(m_Texture.get(), alpha);
	}

	int Texture::getAlphaMod() {
		uint8_t alpha{0};
		SDL_GetTextureAlphaMod(m_Texture.get(), &alpha);
		return alpha;
	}

	int Texture::getBlendMode() {
		SDL_BlendMode blendMode;
		SDL_GetTextureBlendMode(m_Texture.get(), &blendMode);
		return blendMode;
	}

	int Texture::setBlendMode(int blendMode) {
		return SDL_SetTextureBlendMode(m_Texture.get(),
		                               static_cast<SDL_BlendMode>(blendMode));
	}

	uint32_t Texture::getColorMod() {
		uint8_t r, g, b;

		SDL_GetTextureColorMod(m_Texture.get(), &r, &g, &b);

		uint32_t format;
		SDL_QueryTexture(m_Texture.get(), &format, nullptr, nullptr, nullptr);

		SDL_PixelFormat *pixelFormat{SDL_AllocFormat(format)};

		uint32_t colorMod{SDL_MapRGB(pixelFormat, r, g, b)};

		SDL_FreeFormat(pixelFormat);

		return colorMod;
	}

	uint32_t *Texture::getPixelsRW() {
		if (m_LockedRect) {
			return m_PixelsRO.data() + m_LockedRect->x +
			       m_LockedRect->w * sizeof(uint32_t);
		} else {
			return m_PixelsRO.data();
		}
	}

	uint32_t Texture::getNativeAlphaFormat(SDL_Renderer *renderer) {
		SDL_RendererInfo info;
		SDL_GetRendererInfo(renderer, &info);
		for (uint32_t i{0}; i < info.num_texture_formats; ++i) {
			if (!SDL_ISPIXELFORMAT_FOURCC(info.texture_formats[i]) &&
			    SDL_ISPIXELFORMAT_ALPHA(info.texture_formats[i]))
				return info.texture_formats[i];
		}
		return SDL_PIXELFORMAT_UNKNOWN;
	}

	void Texture::setRGBAMasks(){
		SDL_PixelFormat* fmt{SDL_AllocFormat(m_Format)};
		m_Rmask=fmt->Rmask;
		m_Gmask=fmt->Gmask;
		m_Bmask=fmt->Bmask;
		m_Amask=fmt->Amask;
	}

	Texture &Texture::operator=(Texture &&texture){
		if (&texture == this)
			return *this;

		Reset();
		texture.unlock();
		m_Texture = std::move(texture.m_Texture);
		m_Access = std::move(texture.m_Access);
		m_PixelsRO = std::move(texture.m_PixelsRO);

		texture.Reset();

		return *this;
	}

} // namespace RTE