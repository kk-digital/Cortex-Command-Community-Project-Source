#include "SDLTexture.h"
#include "SDLHelper.h"
namespace RTE {
	Texture::Texture(Texture &&texture) {
		texture.unlock();
		m_Texture = std::move(texture.m_Texture);
		w = texture.w;
		h = texture.h;
		m_Access = texture.m_Access;
		m_PixelsRO = std::move(texture.m_PixelsRO);
		texture.Reset();
	}

	void Texture::Reset() {
		m_Texture.reset();
		w = 0;
		h = 0;
		m_Access = SDL_TEXTUREACCESS_STATIC;
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

	int Texture::lock() {
		return SDL_LockTexture(m_Texture.get(), nullptr, &m_PixelsWO, &m_Pitch);
	}

	int Texture::lock(const SDL_Rect &region) {
		return SDL_LockTexture(m_Texture.get(), &region, &m_PixelsWO, &m_Pitch);
	}

	void Texture::unlock() {
		SDL_UnlockTexture(m_Texture.get());
		m_PixelsWO = nullptr;
		m_Pitch = 0;
	}

	uint32_t Texture::getPixel(int x, int y) {
		if (x < w && y < h)
			return m_PixelsRO[y * w + x];

		return 0x00FFFFFF;
	}

	uint32_t Texture::getPixel(SDL_Point pos) { return getPixel(pos.x, pos.y); }

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
