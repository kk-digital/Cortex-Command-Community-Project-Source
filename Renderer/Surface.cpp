#include "Surface.h"
#include <SDL2/SDL.h>
#include "GLPalette.h"
#include "SDL2_rotozoom.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace RTE {
	void sdl_surface_deleter::operator()(SDL_Surface *p) { SDL_FreeSurface(p); }

	Surface::Surface() :
	    m_Width(0),
	    m_Height(0),
	    m_BPP(-1),
	    m_Pixels(nullptr),
	    m_Palette(nullptr) {}

	Surface::Surface(const Surface &ref) :
	    m_Width(ref.m_Width), m_Height(ref.m_Height), m_BPP(ref.m_BPP), m_BlendMode(ref.m_BlendMode), m_Palette(ref.m_Palette) {
		m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(SDL_ConvertSurface(ref.m_Pixels.get(), ref.m_Pixels->format, 0));
	}

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

	bool Surface::Create(SDL_Surface *pixels, std::optional<std::shared_ptr<Palette>> palette) {
		m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(SDL_ConvertSurface(pixels, pixels->format, 0));
		if (palette)
			m_Palette = *palette;

		m_BPP = m_Pixels->format->BitsPerPixel;
		m_Width = m_Pixels->w;
		m_Height = m_Pixels->h;

		m_BlendMode = BlendModes::Blend;

		return static_cast<bool>(m_Pixels);
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
		return static_cast<uint32_t *>(m_Pixels->pixels)[y * m_Width + x];
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
		static_cast<uint32_t *>(m_Pixels->pixels)[x + y * m_Width] = color;
	}

	void Surface::ClearColor(uint32_t color) {
		if(m_BPP == 8) {
			std::fill(static_cast<unsigned char*>(m_Pixels->pixels), static_cast<unsigned char*>(m_Pixels->pixels) + (m_Pixels->pitch * m_Pixels->h), static_cast<unsigned char>(color));
		} else {
			std::fill(static_cast<uint32_t*>(m_Pixels->pixels), static_cast<uint32_t*>(m_Pixels->pixels) + (m_Pixels->pitch * m_Pixels->h), color);
		}
	}

	void Surface::blit(std::shared_ptr<Surface> target, int x, int y, double angle, float scaleX, float scaleY) const {
		SDL_Surface *rotateScaled = rotozoomSurfaceXY(m_Pixels.get(), angle, scaleX, scaleY, SMOOTHING_OFF);
		SDL_Rect dest{x, y, rotateScaled->w, rotateScaled->h};
		SDL_BlitSurface(rotateScaled, nullptr, target->GetPixels(), &dest);
		SDL_FreeSurface(rotateScaled);
	}

	void Surface::blit(std::shared_ptr<Surface> target, glm::mat4 model) {
		glm::quat orientation;
		glm::vec3 scale;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(model, scale, orientation, translation, skew, perspective);
		float angle = glm::angle(orientation);

		blit(target, translation.x, translation.y, angle, scale.x, scale.y);
	}

	void Surface::blit(std::shared_ptr<Surface> target, glm::vec2 position) const {
		assert(target.get());
		assert(target->GetPixels());

		SDL_Rect dest{static_cast<int>(position.x), static_cast<int>(position.y), m_Width, m_Height};
		SDL_BlitSurface(m_Pixels.get(), nullptr, target->GetPixels(), &dest);
	}

	void Surface::blit(std::shared_ptr<Surface> target, std::optional<glm::vec4> srcRect, glm::vec4 destRect) const {
		SDL_Rect src{0, 0, m_Width, m_Height};
		if (srcRect) {
			src.x = srcRect->x;
			src.y = srcRect->y;
			src.w = srcRect->z;
			src.h = srcRect->w;
		}

		SDL_Rect dest{static_cast<int>(destRect.x), static_cast<int>(destRect.y), static_cast<int>(destRect.z), static_cast<int>(destRect.w)};

		SDL_BlitScaled(m_Pixels.get(), &src, target->GetPixels(), &dest);
	}

	void Surface::blitMasked(std::shared_ptr<Surface> target, uint32_t color, int x, int y, double angle, float scaleX, float scaleY) const {
		SDL_Surface *masked = nullptr;

		if (m_BPP == 8) {
			std::vector<unsigned char> maskedPixels;
			maskedPixels.resize(m_Pixels->pitch * m_Pixels->h);
			for (size_t i = 0; i < m_Pixels->pitch * m_Pixels->h; ++i) {
				maskedPixels[i] = static_cast<unsigned char *>(m_Pixels->pixels)[i] == 0 ? 0 : color;
			}

			masked = SDL_CreateRGBSurfaceWithFormatFrom(static_cast<void *>(maskedPixels.data()), m_Width, m_Height, m_BPP, m_Pixels->pitch, m_Pixels->format->format);

			SDL_Surface *rotateScaled = rotozoomSurfaceXY(masked, angle, scaleX, scaleY, SMOOTHING_OFF);

			SDL_Rect dest{x, y, rotateScaled->w, rotateScaled->h};
			SDL_BlitSurface(rotateScaled, nullptr, target->GetPixels(), &dest);
			SDL_FreeSurface(rotateScaled);
		} else {
			assert(m_Pixels->format->BitsPerPixel == 32);

			std::vector<uint32_t> maskedPixels(m_Pixels->pitch * m_Pixels->h);

			for (int i = 0; i < m_Pixels->pitch * m_Pixels->h; ++i) {
				maskedPixels[i] = (static_cast<uint32_t *>(m_Pixels->pixels)[i] & m_Pixels->format->Amask) == 0 ? 0 : color;
			}

			masked = SDL_CreateRGBSurfaceWithFormatFrom(static_cast<void *>(maskedPixels.data()), m_Width, m_Height, m_BPP, m_Pixels->pitch, m_Pixels->format->format);

			SDL_Surface *rotateScaled = rotozoomSurfaceXY(masked, angle, scaleX, scaleY, SMOOTHING_OFF);

			SDL_Rect dest{x, y, rotateScaled->w, rotateScaled->h};
			SDL_BlitSurface(rotateScaled, nullptr, target->GetPixels(), &dest);
			SDL_FreeSurface(rotateScaled);
		}

		if (masked) {
			SDL_FreeSurface(masked);
		}
	}

	void Surface::blitMasked(std::shared_ptr<Surface> target, uint32_t color, glm::mat4 model) {
		glm::quat orientation;
		glm::vec3 scale;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(model, scale, orientation, translation, skew, perspective);
		float angle = glm::angle(orientation);

		blitMasked(target, color, translation.x, translation.y, angle, scale.x, scale.y);
	}

	void Surface::fillRect(glm::vec4 rect, uint32_t color) {
		SDL_Rect sdlRect{static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.z), static_cast<int>(rect.w)};
		SDL_FillRect(m_Pixels.get(), &sdlRect, color);
	}

	void Surface::rect(glm::vec4 rect, uint32_t color) {
		std::vector<SDL_Rect> rects{
		    {static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.z), 1},
		    {static_cast<int>(rect.x), static_cast<int>(rect.y), 1, static_cast<int>(rect.w)},
		    {static_cast<int>(rect.x), static_cast<int>(rect.y + rect.w), static_cast<int>(rect.z), 1},
		    {static_cast<int>(rect.x + rect.z), static_cast<int>(rect.y), 1, static_cast<int>(rect.w)}};
		SDL_FillRects(m_Pixels.get(), rects.data(), 4, color);
	}
} // namespace RTE
