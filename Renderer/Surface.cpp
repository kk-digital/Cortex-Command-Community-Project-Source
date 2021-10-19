#include "Surface.h"
#include <SDL2/SDL.h>
#include "GLPalette.h"
#include "FrameMan.h"

namespace RTE {
	Surface::Surface() :
	    m_Width(0),
	    m_Height(0),
	    m_BPP(-1),
	    m_Pixels(nullptr),
	    m_Palette(nullptr) {}

	Surface::~Surface() = default;

	bool Surface::Create(int width, int height, BitDepth format) {
		uint32_t sdlFormat = (format == BitDepth::Indexed8) ? SDL_PIXELFORMAT_INDEX8 : SDL_PIXELFORMAT_RGBA32;
		m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(sdlFormat), sdlFormat));

		m_Width = width;
		m_Height = height;
		m_BPP = SDL_BITSPERPIXEL(sdlFormat);
		if (m_BPP == 8) {
			m_Palette = g_FrameMan.GetDefaultPalette();
			SDL_SetSurfacePalette(m_Pixels.get(), m_Palette->GetAsPalette());
		}
		return m_Pixels.get();
	}

	uint32_t Surface::GetPixel(int x, int y) { return 0; }

	void Surface::blit(Surface &target, int x, int y, double angle, float scaleX, float scaleY) const {}

	void Surface::blitColor(Surface &target, uint32_t color, int x, int y, double angle, float scaleX, float scaleY) const {}
} // namespace RTE
