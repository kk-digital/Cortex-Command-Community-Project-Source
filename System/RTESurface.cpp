#include "RTESurface.h"
#include "SDLHelper.h"
#include "SDLTexture.h"
namespace RTE {

	Surface::Surface(int width, int height) :
	    w{width}, h{height} {
		m_Pixels = std::vector<uint32_t>(width * height, 0);
	}

	void Surface::setPixel(int x, int y, uint32_t color) {
		if (x < w && y < h) {
			m_Pixels[(y * w) + x] = color;
		}
	}

	uint32_t Surface::GetPixel(int x, int y) {
		if (x < w && y < h)
			return m_Pixels[(y * w) + x];
		return 0xFFFFFF00;
	}

} // namespace RTE
