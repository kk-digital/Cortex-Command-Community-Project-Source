#ifndef _RTERENDERTARGET_
#define _RTERENDERTARGET_
#include "RenderState.h"
#include "VertexArray.h"
namespace RTE {

	class RenderTarget {
	public:
		RenderTarget();
		virtual ~RenderTarget();

		virtual void Create(int x, int y, int width, int height);

		void Draw(RenderState& state);

	private:
		VertexArray m_DefaultQuad; //!< The default unit rectangle, used for drawing sprites without primitive attachments.
		glm::mat4 m_Projection; //!< Orthographic projection matrix to transform screen coords to normalized screen coordinates.
		glm::mat4 m_View; //!< Currently unused, because we implement the camera differently (badly).
	};
}
#endif
