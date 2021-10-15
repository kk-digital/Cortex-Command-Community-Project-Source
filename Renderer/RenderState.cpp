#include "RenderState.h"
#include "VertexArray.h"
#include "GLTexture.h"
#include "Shader.h"

#include "GL/glew.h"

namespace RTE {
	RenderState::RenderState() :
	    m_Texture(0),
	    m_Color(1),
	    m_ModelTransform(1),
	    m_BlendMode(BlendModes::Blend),
	    m_PrimitiveType(PrimitiveType::Triangle),
	    m_Shader(nullptr) {}

	RenderState::RenderState(GLTexture* texture, glm::mat4 modelTransform, std::shared_ptr<Shader> shader, BlendMode blendMode, glm::vec4 colorMod) :
		m_Vertices(nullptr),
	    m_Texture(texture),
	    m_Color(colorMod),
	    m_ModelTransform(modelTransform),
	    m_BlendMode(blendMode),
	    m_PrimitiveType(PrimitiveType::Triangle),
	    m_Shader(shader) {}

	RenderState::RenderState(glm::vec4 color, VertexArray* vertexArray, glm::mat4 modelTransform) :
		m_Vertices(vertexArray),
	    m_Texture(0),
	    m_Color(color),
	    m_ModelTransform(modelTransform),
	    m_BlendMode(BlendModes::Blend),
	    m_PrimitiveType(PrimitiveType::Triangle),
	    m_Shader(nullptr) {}
} // namespace RTE
