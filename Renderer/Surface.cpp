#include "Surface.h"
#include <SDL2/SDL.h>
#include "GLPalette.h"

namespace RTE {
	Surface::Surface() :
	    m_Width(0),
	    m_Height(0),
	    m_BPP(-1),
	    m_Pixels(nullptr),
	    m_Palette(nullptr) {}

	Surface::~Surface() = default;

	bool Surface::Create(int width, int height, BitDepth format, std::optional<std::shared_ptr<Palette>> palette) {
		uint32_t sdlFormat = (format == BitDepth::Indexed8) ? SDL_PIXELFORMAT_INDEX8 : SDL_PIXELFORMAT_RGBA32;
		m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(sdlFormat), sdlFormat));

		m_Width = width;
		m_Height = height;
		m_BPP = SDL_BITSPERPIXEL(sdlFormat);
		if (m_BPP == 8) {
			if (palette)
				m_Palette = *palette;
			else
				m_Palette = std::make_shared<Palette>(std::array<glm::u8vec4, Palette::PALETTESIZE>());
			SDL_SetSurfacePalette(m_Pixels.get(), m_Palette->GetAsPalette());
		}
		return m_Pixels.get();
	}

	uint32_t Surface::GetPixel(int x, int y) {
		if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
			return PixelOutside;
		else
			return m_BPP == 8 ? GetPixel8(x, y) : GetPixel32(x, y);
	}

	int Surface::GetPixel8(int x, int y) {
		return static_cast<unsigned char *>(m_Pixels->pixels)[y * m_Pixels->pitch + x];
	}

	uint32_t Surface::GetPixel32(int x, int y) {
		return static_cast<uint32_t *>(m_Pixels->pixels)[y * m_Pixels->pitch + x];
	}

	void Surface::SetPixel(int x, int y, glm::u8vec4 color) {
		if (m_BPP == 32)
			SetPixel(x, y, ((color.a << 24)) | (color.r << 16) | (color.g << 8) | (color.b));
		else {
			SDL_PixelFormat *indexed{SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8)};
			SDL_SetPixelFormatPalette(indexed, m_Palette->GetAsPalette());
			uint32_t index{SDL_MapRGBA(indexed, color.r, color.g, color.b, color.a)};
			SetPixel(x, y, index);
		}
	}

	void Surface::SetPixel(int x, int y, uint32_t color) {
		if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
			return;

		if (m_BPP == 8)
			SetPixel8(x, y, color);
		else
			SetPixel32(x, y, color);
	}

	void Surface::SetPixel8(int x, int y, unsigned char color) {
		static_cast<unsigned char *>(m_Pixels->pixels)[x + y * m_Pixels->pitch] = color;
	}

	void Surface::SetPixel32(int x, int y, uint32_t color) {
		static_cast<uint32_t *>(m_Pixels->pixels)[x + y * m_Pixels->pitch] = color;
	}

	void Surface::blit(std::shared_ptr<Surface> target, int x, int y, double angle, float scaleX, float scaleY) const {}

	void Surface::blit(std::shared_ptr<Surface> target, glm::vec2 position) const {}

	void Surface::blitMasked(std::shared_ptr<Surface> target, uint32_t color, int x, int y, double angle, float scaleX, float scaleY) const {}
} // namespace RTE
