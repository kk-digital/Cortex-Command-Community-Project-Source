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

		void Draw(RenderState& state) override;

		void DrawClear() override;

		std::shared_ptr<GLTexture> GetAsTexture() { return m_Texture; }

	private:
		using RenderTarget::Create;
		std::shared_ptr<GLTexture> m_Texture; //!< Texture attachment to the framebuffer.
		unsigned int m_FBO; //!< The framebuffer object that gets rendered to.
	};
}
#endif
