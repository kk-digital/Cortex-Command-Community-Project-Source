#ifndef _RTERENDERSTATE_
#define _RTERENDERSTATE_
#include "BlendMode.h"
#include "GLPrimitives.h"
namespace RTE {
	class Shader;
	class VertexArray;
	class GLTexture;
	struct RenderState{
	public:
		RenderState();
		RenderState(GLTexture* texture, glm::mat4 modelTransform, std::shared_ptr<Shader> shader, BlendMode blendMode, glm::vec4 colorMod);

		RenderState(glm::vec4 color, std::shared_ptr<VertexArray> vertexArray, glm::mat4 modelTransform);

		std::shared_ptr<VertexArray> m_Vertices;
		GLTexture* m_Texture;
		glm::vec4 m_Color;
		glm::mat4 m_ModelTransform;
		BlendMode m_BlendMode;
		PrimitiveType m_PrimitiveType;
		std::shared_ptr<Shader> m_Shader;
	};

}
#endif
