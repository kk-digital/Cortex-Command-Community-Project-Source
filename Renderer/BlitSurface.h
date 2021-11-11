#ifndef _RTEBLITSURFACE_
#define _RTEBLITSURFACE_
#include "RenderTarget.h"
namespace RTE {
	class Surface;
	/// <summary>
	///
	/// </summary>
	class BlitSurface : public RenderTarget {
	public:
		BlitSurface(std::shared_ptr<Surface> surface);
		virtual ~BlitSurface();


		virtual bool Create(std::shared_ptr<Surface> surface);
		virtual void SetSurface(std::shared_ptr<Surface> surface);
		virtual void Draw(RenderState &state) override;
	private:
		using RenderTarget::Create;

		std::shared_ptr<Surface> m_SurfaceAttachment; //!< The attached surface to this rendertarget.
	};
} // namespace RTE
#endif
