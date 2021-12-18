#include "BlitSurface.h"
#include "GLTexture.h"
#include "Surface.h"
#include "GLPalette.h"
namespace RTE {
	BlitSurface::BlitSurface() {
		m_SurfaceAttachment = nullptr;
	}

	BlitSurface::BlitSurface(std::shared_ptr<Surface> surface) {
		m_SurfaceAttachment = surface;
	}

	BlitSurface::~BlitSurface() {}

	bool BlitSurface::Create(std::shared_ptr<Surface> surface) {
		m_SurfaceAttachment = surface;
		return m_SurfaceAttachment.get();
	}

	void BlitSurface::SetSurface(std::shared_ptr<Surface> surface) {
		m_SurfaceAttachment = surface;
	}

	void BlitSurface::Draw(RenderState &state) {
		switch (state.m_Texture->getShading()) {
			case Shading::Fill: {
				uint32_t color;
				if (state.m_Texture->GetBitDepth() == 8) {
					color = state.m_Texture->GetPalette()->GetIndexFromColor(state.m_Color);
				} else {
					color = static_cast<uint32_t>(state.m_Color.a * 255) << 24 |
					        static_cast<uint32_t>(state.m_Color.r * 255) << 16 |
					        static_cast<uint32_t>(state.m_Color.g * 255) << 8 |
					        static_cast<uint32_t>(state.m_Color.b * 255);
					state.m_Texture->blitMasked(m_SurfaceAttachment, color, state.m_ModelTransform);
				}
			} break;

			default:
				state.m_Texture->blit(m_SurfaceAttachment, state.m_ModelTransform);
				break;
		}
	}
} // namespace RTE
