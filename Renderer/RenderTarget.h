#ifndef _RTERENDERTARGET_
#define _RTERENDERTARGET_
#include "RenderState.h"
#include "VertexArray.h"
namespace RTE {

	class RenderTarget {
	public:
		RenderTarget();
		virtual ~RenderTarget();

		void draw(RenderState& state);
	private:
		VertexArray m_DefaultQuad; //!< The default unit rectangle, used for drawing sprites without primitive attachments.
		RenderState m_State;
	};
}
#endif
