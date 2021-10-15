#include "Surface.h"
#include <SDL2/SDL.h>

namespace RTE {
	Surface::Surface() :
	    m_Width(0),
	    m_Height(0),
	    m_BPP(-1),
	    m_Pixels(nullptr),
	    m_Palette(256) {}

	Surface::~Surface() = default;

	bool Surface::Create(int width, int height, uint32_t format) {
		m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(format), format));

		m_Width = width;
		m_Height = height;
		m_BPP = SDL_BITSPERPIXEL(format);
		return m_Pixels.get();
	}

	uint32_t Surface::GetPixel(int x, int y) { return 0;}

	void Surface::blit(Surface &target, int x, int y, double angle = 0, float scaleX = 1.0f, float scaleY = 1.0f) const {}

	void Surface::blitColor(Surface &target, uint32_t color, int x, int y, double angle = 0, float scaleX = 1.0f, float scaleY = 1.0f) const {}
} // namespace RTE
