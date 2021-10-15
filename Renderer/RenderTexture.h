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
		bool Create(int width, int height, uint32_t format);

	private:
		std::shared_ptr<GLTexture> m_Texture; //!< Texture attachment to the framebuffer.
		unsigned long m_FBO; //!< The framebuffer object that gets rendered to.
	};
}
#endif
