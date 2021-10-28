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

		/// <summary>
		/// Draws the passed state to the screen (or the active render target which should be the screen).
		/// </summary>
		/// <param name="state">
		/// The render state that should be applied.
		/// </param>
		// TODO: States caching.
		virtual void Draw(RenderState &state);

		/// <summary>
		/// Clear the current render target.
		/// </summary>
		virtual void DrawClear();

		/// <summary>
		/// Get the dimensions in pixels of the render target.
		/// </summary>
		/// <returns>
		/// A vector containing the width and height of the render target.
		/// </returns>
		virtual glm::vec2 GetSize() { return m_Size; }

		virtual glm::vec2 GetViewport() { return m_View.zw(); }

	protected:
		glm::mat4 m_Projection; //!< Orthographic projection matrix to transform screen coords to normalized screen coordinates.
		glm::vec4 m_View; //!< Current view of this render target. //TODO: implement

		glm::vec2 m_Size; //!< Size in pixels of this rendertarget.
	private:
		VertexArray m_DefaultQuad; //!< The default unit rectangle, used for drawing sprites without primitive attachments.
	};
} // namespace RTE
#endif
