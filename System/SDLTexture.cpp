#include "SDLTexture.h"
#include "SDLHelper.h"

#include "Constants.h"

#include "RTEError.h"

void sdl_format_deleter::operator()(SDL_PixelFormat *p) { SDL_FreeFormat(p); }
void sdl_texture_deleter::operator()(SDL_Texture *p) { SDL_DestroyTexture(p); }

namespace RTE {

	Texture::Texture() {
		Reset();
	}

	Texture::Texture(Texture &&texture) {
		texture.unlock();
		m_Texture = std::move(texture.m_Texture);
		w = texture.w;
		h = texture.h;
		m_Format = texture.m_Format;
		m_Access = texture.m_Access;
		m_PixelsRO = std::move(texture.m_PixelsRO);
		m_PixelsWO = nullptr;
		m_Pitch = 0;
		texture.Reset();
	}

	Texture::Texture(SDL_Renderer *renderer, const Texture &texture) {
		Reset();
		std::copy(texture.m_PixelsRO.begin(), texture.m_PixelsRO.end(), std::back_inserter(m_PixelsRO));
		w = texture.w;
		h = texture.h;

		m_Format = getNativeAlphaFormat(renderer);

		m_Texture = std::unique_ptr<SDL_Texture, sdl_texture_deleter>(SDL_CreateTexture(renderer, m_Format, texture.m_Access, texture.w, texture.h));

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

	Texture::Texture(SDL_Renderer *renderer, int width, int height, int access) :
	    w{width}, h{height}, m_Access{access} {

		m_Format = getNativeAlphaFormat(renderer);
		m_PixelsWO = nullptr;
		m_Pitch = 0;
		m_PixelsRO.clear();
		m_PixelsRO.resize(width * height);
		m_Texture = std::unique_ptr<SDL_Texture, sdl_texture_deleter>(SDL_CreateTexture(renderer, m_Format, access, width, height));
		RTEAssert(m_Texture.get(), "Texture was not created because: " + std::string(SDL_GetError()));
		SDL_SetTextureBlendMode(m_Texture.get(), SDL_BLENDMODE_BLEND);
	}

	Texture::~Texture() { Reset(); };

	void Texture::Reset() {
		m_Texture.reset();
		w = 0;
		h = 0;
		m_Access = SDL_TEXTUREACCESS_STATIC;
		m_Format = SDL_PIXELFORMAT_UNKNOWN;
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

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &source, const SDL_Rect &dest) {
		return SDL_RenderCopy(pRenderer, m_Texture.get(), &source, &dest);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, nullptr, SDL_FLIP_NONE);
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y, double angle) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, angle);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest, int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest, 0,
		                        nullptr, static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y, int flip) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y, double angle, int flip) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, angle, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer,
	                    int x, int y,
	                    double angle, const SDL_Point &center,
	                    int flip) {
		return render(pRenderer, SDL_Rect{x, y, w, h}, angle, center, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer,
	                    int x, int y,
	                    double angle,
	                    int flip,
	                    double scale) {
		return render(pRenderer, SDL_Rect{x, y, w, h} * scale, angle, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer,
	                    int x, int y,
	                    double angle, const SDL_Point &center,
	                    int flip,
	                    double scale) {
		return render(pRenderer, SDL_Rect{x, y, w, h} * scale, angle, center, flip);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle, int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest, angle, nullptr, static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest, double angle, const SDL_Point &center, int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, &center,
		                        static_cast<SDL_RendererFlip>(flip));
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y, uint32_t color) {
		return renderFillColor(renderer, SDL_Rect{0, 0, w, h},
		                       SDL_Rect{x, y, w, h}, color);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, const SDL_Rect &source, const SDL_Rect &dest, uint32_t color) {
		InitializeSilhouette(renderer);
		silhouetteTexture->setColorMod((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF);
		silhouetteTexture->setAlphaMod(color & 0xFF);

		// Render the texture area from the intermediate target to the screen
		return silhouetteTexture->render(renderer, source, dest);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y, uint32_t color, double angle) {
		return renderFillColor(renderer, SDL_Rect{x, y, w, h}, color, angle, SDL_Point{w / 2, h / 2}, SDL_FLIP_NONE);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y, uint32_t color, double angle, int flip) {
		return renderFillColor(renderer, SDL_Rect{x, y, w, h}, color, angle, SDL_Point{w / 2, h / 2}, flip);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y, uint32_t color, double angle, int flip, double scale) {
		SDL_Rect scaleDest{x, y, w, h};
		scaleDest.w *= scale;
		scaleDest.h *= scale;

		return renderFillColor(renderer, scaleDest, color, angle, SDL_Point{w / 2, h / 2}, flip);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer, int x, int y, uint32_t color, double angle, const SDL_Point &center, int flip, double scale) {
		SDL_Rect scaleDest{x, y, w, h};
		scaleDest.w *= scale;
		scaleDest.h *= scale;

		return renderFillColor(renderer, scaleDest, color, angle, center, flip);
	}

	int Texture::renderFillColor(SDL_Renderer *renderer,
	                             const SDL_Rect &dest, uint32_t color,
	                             double angle, const SDL_Point &center, int flip) {
		InitializeSilhouette(renderer);

		silhouetteTexture->setColorMod((color >> 16) & 0xFF, (color >> 8) & 0xFF, (color)&0xFF);
		silhouetteTexture->setAlphaMod(color & 0xFF000000);

		// Render the texture area from the intermediate target to the screen
		return silhouetteTexture->render(renderer, dest, angle, center, flip);
	}

	int Texture::lock() {
		if (m_PixelsWO)
			return 0;
		assert(m_Access == SDL_TEXTUREACCESS_STREAMING);
		m_Updated = false;
		int result = SDL_LockTexture(m_Texture.get(), nullptr, &m_PixelsWO, &m_Pitch);
		assert(m_PixelsWO);
		return result;
	}

	// int Texture::lock(const SDL_Rect &region) {
	// 	if (lock())
	// 		return -1;
	// 	if (m_Access == SDL_TEXTUREACCESS_STREAMING) {
	// 		m_LockedRect = std::make_unique<SDL_Rect>(SDL_Rect{region.x, region.y, 0, 0});

	// 		if ((region.x + region.w) < w)
	// 			m_LockedRect->w = region.w;
	// 		else
	// 			m_LockedRect->w = w - region.x;
	// 		if ((region.y + region.h) < h)
	// 			m_LockedRect->h = region.h;
	// 		else
	// 			m_LockedRect->h = h - region.y;

	// 		m_Updated = true;
	// 		return 0;
	// 	}
	// 	return -1;
	// }

	void Texture::unlock() {
		if (m_PixelsWO) {
			if (m_LockedRect || m_Updated) {
				SDL_Surface *surfaceWO{SDL_CreateRGBSurfaceWithFormatFrom(m_PixelsWO, w, h, 32, m_Pitch, m_Format)};
				auto surfaceRO{getPixelsAsSurface()};
				SDL_BlitSurface(surfaceRO.get(), nullptr, surfaceWO, nullptr);
				SDL_FreeSurface(surfaceWO);
			}
			SDL_UnlockTexture(m_Texture.get());
			m_PixelsWO = nullptr;
			m_Pitch = 0;
		}
	}

	uint32_t Texture::getPixelLower(int x, int y) const {
		assert(x >= 0 && y >= 0 && x < w && y < h);
		return m_PixelsRO[y * w + x];
	}

	uint32_t Texture::getPixel(int x, int y) const {
		if (x >= 0 && y >= 0 && x < w && y < h) {
			return getPixelLower(x, y);
		}

		return 0x00FFFFFF;
	}

	uint32_t Texture::getPixel(SDL_Point pos) const {
		return getPixel(pos.x, pos.y);
	}

	uint32_t Texture::getPixel(size_t index) const {
		if (index < m_PixelsRO.size()) {
			return m_PixelsRO[index];
		} else {
			return 0x00FFFFFF;
		}
	}

	void Texture::setPixel(int x, int y, uint32_t color) {
		assert(m_PixelsWO);
		if (m_PixelsWO && x >= 0 && y >= 0 && x < w && y < h) {
			setPixelLower(x, y, color);
		}
	}

	void Texture::setPixel(int x, int y, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		assert(m_PixelsWO);
		setPixel(x, y, (a << 24) | (r << 16) | (g << 8) | (b));
	}

	void Texture::setPixelLower(int x, int y, uint32_t color) {
		assert(m_PixelsWO);
		m_PixelsRO[y * w + x] = color;
		static_cast<uint32_t*>(m_PixelsWO)[y * w + x] = color;
	}

	void Texture::setPixels(const SDL_Rect &setRect, const std::vector<uint32_t> &pixels) {
		assert(m_PixelsWO);
		SDL_Rect clippedRect;
		SDL_Rect dimensions{0, 0, w, h};
		if (!SDL_IntersectRect(&dimensions, &setRect, &clippedRect))
			return;
		int xOffset = std::abs(setRect.x - clippedRect.x);
		for (int y = 0; y < clippedRect.h; ++y) {
			std::copy(pixels.begin() + ( setRect.w * y ) + xOffset, pixels.begin() + (setRect.w * y) + clippedRect.w - 1, m_PixelsRO.begin() + ((clippedRect.y + y) * w + clippedRect.x));
			std::copy(pixels.begin() + ( setRect.w * y ) + xOffset, pixels.begin() + (setRect.w * y) + clippedRect.w - 1, static_cast<uint32_t *>(m_PixelsWO) + ((clippedRect.y + y) * w + clippedRect.x));
		}
	}

	void Texture::remapColor(uint32_t oldColor, uint32_t newColor) {
		if (!m_PixelsWO)
			return;

		SDL_PixelFormat *pf{SDL_AllocFormat(m_Format)};

		uint32_t oldColorFmt{SDL_MapRGBA(pf, (oldColor >> 24) & 0xFF, (oldColor >> 16) & 0xFF, (oldColor >> 8) & 0xFF, oldColor & 0xFF)};
		uint32_t newColorFmt{SDL_MapRGBA(pf, (newColor >> 24) & 0xFF, (newColor >> 16) & 0xFF, (newColor >> 8) & 0xFF, oldColor & 0xFF)};

		std::replace(m_PixelsRO.begin(), m_PixelsRO.end(), oldColorFmt, newColorFmt);

		SDL_FreeFormat(pf);

		m_Updated = true;
	}

	void Texture::clearAll(uint32_t color) {
		uint32_t fillColor{color};
		if (fillColor) {
			SDL_PixelFormat *pf{SDL_AllocFormat(m_Format)};
			fillColor = SDL_MapRGBA(pf, (color >> 24) & 0xff, (color >> 16) & 0xff, (color >> 8) & 0xff, (color)&0xff);
			SDL_FreeFormat(pf);
		}

		std::fill(m_PixelsRO.begin(), m_PixelsRO.end(), fillColor);
		if (m_PixelsWO)
			SDL_ConvertPixels(w, h, m_Format, m_PixelsRO.data(), sizeof(uint32_t), m_Format, m_PixelsWO, m_Pitch);
	}

	void Texture::fillRect(const SDL_Rect &rect, uint32_t color) {
		assert(m_PixelsWO);
		if (!m_LockedRect && m_PixelsWO) {
			std::unique_ptr<SDL_Surface, sdl_deleter> temp{
			    SDL_CreateRGBSurfaceWithFormatFrom(getPixels(), w, h, 32,
			                                       sizeof(uint32_t), m_Format)};
			SDL_FillRect(temp.get(), &rect, color);
			SDL_ConvertPixels(w, h, m_Format, getPixels(),
			                  w * sizeof(uint32_t), m_Format, m_PixelsWO,
			                  m_Pitch);
		}
	}

	void Texture::rect(const SDL_Rect &rect, uint32_t color) {
		assert(m_PixelsWO);
		if (m_PixelsWO) {
			std::unique_ptr<SDL_Surface, sdl_deleter> temp{getPixelsAsSurface()};
			SDL_Rect topEdge{rect.x, rect.y, rect.w, 1};
			SDL_Rect rightEdge{rect.x + rect.w - 1, rect.y, 1, rect.h};
			SDL_Rect leftEdge{rect.x, rect.y, 1, rect.h};
			SDL_Rect bottomEdge{rect.x, rect.y + rect.h - 1, rect.w, 1};
			SDL_FillRect(temp.get(), &topEdge, color);
			SDL_FillRect(temp.get(), &leftEdge, color);
			SDL_FillRect(temp.get(), &bottomEdge, color);
			SDL_FillRect(temp.get(), &rightEdge, color);
		}
	}

	void Texture::fillLocked(uint32_t color) {
		if (m_PixelsWO && m_LockedRect) {
			SDL_PixelFormat *pf{SDL_AllocFormat(m_Format)};
			uint32_t formattedColor = SDL_MapRGBA(pf, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color)&0xFF);
			SDL_FreeFormat(pf);
			std::unique_ptr<SDL_Surface, sdl_deleter> temp{
			    SDL_CreateRGBSurfaceWithFormatFrom(getPixels(), w, h, 32,
			                                       w * sizeof(32), m_Format)};
			SDL_FillRect(temp.get(), nullptr, formattedColor);
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

		SDL_PixelFormat *pf{SDL_AllocFormat(m_Format)};
		uint32_t colorMod{SDL_MapRGB(pf, r, g, b)};
		SDL_FreeFormat(pf);

		return colorMod;
	}

	int Texture::setColorMod(uint8_t r, uint8_t g, uint8_t b) {
		return SDL_SetTextureColorMod(m_Texture.get(), r, g, b);
	}

	uint32_t *Texture::getPixels() {
		m_Updated = true;
		if (m_LockedRect) {
			return m_PixelsRO.data() + m_LockedRect->x +
			       m_LockedRect->w * sizeof(uint32_t);
		} else {
			return m_PixelsRO.data();
		}
	}

	std::unique_ptr<SDL_Surface, sdl_deleter> Texture::getPixelsAsSurface() {
		m_Updated = true;
		if (!m_LockedRect)
			return std::unique_ptr<SDL_Surface, sdl_deleter>(SDL_CreateRGBSurfaceWithFormatFrom(getPixels(), w, h, 32, w * sizeof(uint32_t), m_Format));
		else
			return std::unique_ptr<SDL_Surface, sdl_deleter>(SDL_CreateRGBSurfaceWithFormatFrom(getPixels(), m_LockedRect->w, m_LockedRect->h, 32, w * sizeof(uint32_t), m_Format));
	}

	uint32_t Texture::getNativeAlphaFormat(SDL_Renderer *renderer) {
		// I am generally assuming little endian byteorder, since there is (afaik) no system this would or could run on that is big endian (unless we do a DOS port)
		return SDL_PIXELFORMAT_ARGB8888;
	}

	void Texture::InitializeSilhouette(SDL_Renderer *renderer) {
		if (!silhouetteTexture) {
			silhouetteTexture = std::make_unique<Texture>(renderer, w, h, SDL_TEXTUREACCESS_STATIC);
			RTEAssert(silhouetteTexture.get(), "Unable to create internal silhouette");
			silhouetteTexture->m_PixelsRO = m_PixelsRO;
			SDL_PixelFormat *pf{SDL_AllocFormat(m_Format)};
			std::replace_if(
			    silhouetteTexture->m_PixelsRO.begin(), silhouetteTexture->m_PixelsRO.end(), [pf](auto x) { return (x & pf->Amask) != 0; }, 0xFFFFFFFF);
			std::replace_if(
			    silhouetteTexture->m_PixelsRO.begin(), silhouetteTexture->m_PixelsRO.end(), [pf](auto x) { return (x & pf->Amask) == 0; }, 0);
			SDL_FreeFormat(pf);

			SDL_UpdateTexture(silhouetteTexture->m_Texture.get(), nullptr, silhouetteTexture->m_PixelsRO.data(), w * sizeof(uint32_t));
		}
	}

	Texture &Texture::operator=(Texture &&texture) {
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
