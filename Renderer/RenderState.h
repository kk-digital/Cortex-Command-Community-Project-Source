#ifndef _RTERENDERSTATE_
#define _RTERENDERSTATE_
#include "BlendMode.h"
#include "GLPrimitives.h"
namespace RTE {
	class Shader;
	class VertexArray;
	class GLTexture;
	struct RenderState {
	public:
		RenderState();
		RenderState(std::shared_ptr<GLTexture> texture, glm::mat4 modelTransform, std::shared_ptr<Shader> shader, BlendMode blendMode, glm::vec4 colorMod);

		RenderState(glm::vec4 color, std::shared_ptr<VertexArray> vertexArray, glm::mat4 modelTransform);

		std::shared_ptr<VertexArray> m_Vertices; //!< Vertices to draw.
		std::shared_ptr<GLTexture> m_Texture; //!< Texture to draw.
		glm::vec4 m_Color; //!< Color modifier applied to the rteColorMod uniform.
		glm::mat4 m_ModelTransform; //!< translation * rotate * scale in that order.
		glm::mat4 m_UVTransform;
		BlendMode m_BlendMode; //!< Blendmode to use for drawing. (applied after shader stage)
		PrimitiveType m_PrimitiveType; //!< What primitive type to use for drawing the vertices.
		std::shared_ptr<Shader> m_Shader; //!< Shader to use for drawing (must not be nullptr ever).
		int m_FBO; //!< Framebuffer object to draw to.
	};

} // namespace RTE
#endif
