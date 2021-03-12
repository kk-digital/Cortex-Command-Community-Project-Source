#include "SDLTexture.h"
#include "SDLHelper.h"
namespace RTE {
	Texture::Texture(std::string filename, int access) {

	}

	Texture::Texture(SDL_Texture *pTexture, int access) {}

	int Texture::render(SDL_Renderer *pRenderer, int x, int y) {
		return render(pRenderer, SDL_Rect{x, y, w, h});
	}

	int Texture::render(SDL_Renderer *pRenderer, SDL_Point pos) {
		return render(pRenderer, SDL_Rect{pos.x, pos.y, w, h});
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest) {
		return SDL_RenderCopy(pRenderer, m_Texture.get(), nullptr, &dest);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                         double angle) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest,
		                        angle, nullptr, SDL_FLIP_NONE);
	}

	int Texture::render(SDL_Renderer *pRenderer, const SDL_Rect &dest,
	                         int flip) {
		return SDL_RenderCopyEx(pRenderer, m_Texture.get(), nullptr, &dest, 0,
		                        nullptr, static_cast<SDL_RendererFlip>(flip));
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

	int Texture::lock(const SDL_Rect &region){
		return SDL_LockTexture(m_Texture.get(), &region, &m_PixelsWO, &m_Pitch);
	}

	void Texture::unlock() {
		SDL_UnlockTexture(m_Texture.get());
		m_PixelsWO = nullptr;
	}
} // namespace RTE
