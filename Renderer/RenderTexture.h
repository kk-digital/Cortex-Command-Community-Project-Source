#ifndef _RTERENDERTEXTURE_
#define _RTERENDERTEXTURE_
#include "RenderTarget.h"

namespace RTE {
	class GLTexture;
	class RenderTexture : public RenderTarget {
	public:
		RenderTexture();
		virtual ~RenderTexture();

		/// <summary>
		/// Creates the associated texture and framebuffer.
		/// </summary>
		/// <param name="width">
		/// The width of the texture.
		/// </param>
		/// <param name="height">
		/// The height of the texture.
		/// </param>
		/// <param name="format">
		/// The format the attached texture should use.
		/// </param>
		/// <returns>
		/// True  if creation was successful.
		/// </returns>
		void Create(int width, int height, uint32_t format = 0);

		/// <summary>
		/// Draw the passed state to this render target.
		/// </summary>
		/// <param name="state">
		/// The state to draw.
		/// </param>
		void Draw(RenderState &state) override;

		/// <summary>
		/// Clear this texture.
		/// </summary>
		void DrawClear() override;

		/// <summary>
		/// Get the texture attachment of this render target.
		/// </summary>
		/// <returns>
		/// Shared pointer to the texture attachment.
		/// </returns>
		std::shared_ptr<GLTexture> GetAsTexture() { return m_Texture; }

		/// <summary>
		/// Get the width of the attached texture.
		/// </summary>
		/// <returns>
		/// The width in pixels of the texture.
		/// </returns>
		int GetW() const;
		/// <summary>
		/// Get the height of the attached texture.
		/// </summary>
		/// <returns>
		/// The height in pixels of the texture.
		/// </returns>
		int GetH() const;

	private:
		using RenderTarget::Create;
		std::shared_ptr<GLTexture> m_Texture; //!< Texture attachment to the framebuffer.
		unsigned int m_FBO; //!< The framebuffer object that gets rendered to.
	};
} // namespace RTE
#endif
