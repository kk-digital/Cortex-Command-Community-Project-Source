#ifndef _RTERENDERTEXTURE_
#define _RTERENDERTEXTURE_
#include "RenderTarget.h"
#include "GLTexture.h"

namespace RTE {
	class RenderTexture : public RenderTarget {
	public:
		/// <summary>
		/// Initializes a RenderTexture in memory. Creates the FBO.
		/// </summary>
		RenderTexture();
		virtual ~RenderTexture();

		/// <summary>
		/// Initializes a RenderTexture in memory from an existing RenderTexture. Creates the FBO.
		/// </summary>
		/// <param name="ref">
		/// The reference to get the texture attachment from.
		/// </param>
		RenderTexture(std::shared_ptr<RenderTexture> ref);

		/// <summary>
		/// Initializes a RenderTexture in memory from an existing RenderTexture. Creates the FBO.
		/// </summary>
		/// <param name="texture">
		/// The texture attachment to use.
		/// </param>
		RenderTexture(std::shared_ptr<GLTexture> texture);

		using RenderTarget::Create;

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
		void Create(int width, int height, BitDepth format);

		void Create(std::shared_ptr<GLTexture> texture);

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
		void DrawClear(glm::vec4 color = glm::vec4(0)) override;

		/// <summary>
		/// Get the texture attachment of this render target.
		/// </summary>
		/// <returns>
		/// Shared pointer to the texture attachment.
		/// </returns>
		std::shared_ptr<GLTexture> GetTexture() { return m_Texture; }

		/// <summary>
		/// Set the texture attachment for this render texture.
		/// May be useful for drawing to textures or blitting textures.
		/// </summary>
		/// <param name="texture">
		/// Shared pointer to attach to the framebuffer.
		/// </param>
		void SetTexture(std::shared_ptr<GLTexture> texture);

		bool HasTexture() { return m_Texture.get(); }

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
		std::shared_ptr<GLTexture> m_Texture; //!< Texture attachment to the framebuffer.
		unsigned int m_FBO; //!< The framebuffer object that gets rendered to.
	};
} // namespace RTE
#endif
