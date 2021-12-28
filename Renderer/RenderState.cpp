#include "RenderState.h"
#include "VertexArray.h"
#include "GLTexture.h"
#include "Shader.h"

#include "GL/glew.h"

namespace RTE {
	RenderState::RenderState() :
	    m_Texture(0),
	    m_Color(1.0f),
	    m_ModelTransform(1.0f),
	    m_BlendMode(BlendModes::Blend),
	    m_PrimitiveType(PrimitiveType::TriangleStrip),
	    m_Shader(nullptr),
		m_FBO(0){}

	RenderState::RenderState(std::shared_ptr<GLTexture> texture, glm::mat4 modelTransform, std::shared_ptr<Shader> shader, BlendMode blendMode, glm::vec4 colorMod) :
		m_Vertices(nullptr),
	    m_Texture(texture),
	    m_Color(colorMod),
	    m_ModelTransform(modelTransform),
	    m_BlendMode(blendMode),
	    m_PrimitiveType(PrimitiveType::TriangleStrip),
	    m_Shader(shader),
		m_FBO(0) {}

	RenderState::RenderState(glm::vec4 color, std::shared_ptr<VertexArray> vertexArray, glm::mat4 modelTransform) :
		m_Vertices(vertexArray),
	    m_Texture(0),
	    m_Color(color),
	    m_ModelTransform(modelTransform),
	    m_BlendMode(BlendModes::Blend),
	    m_PrimitiveType(PrimitiveType::TriangleStrip),
	    m_Shader(nullptr),
		m_FBO(0) {}
} // namespace RTE
