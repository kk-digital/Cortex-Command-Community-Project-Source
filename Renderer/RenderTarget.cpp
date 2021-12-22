#include "RenderTarget.h"

#include "System/RTEError.h"

#include "Shader.h"
#include "Vertex.h"
#include "GLTexture.h"

#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"
#include "GLCheck.h"

namespace RTE {
	RenderTarget::RenderTarget() {
		std::vector<Vertex> quad{
		    {0.0f, 1.0f, 0.0f, 1.0f},
		    {1.0f, 1.0f, 1.0f, 1.0f},
		    {0.0f, 0.0f, 0.0f, 0.0f},
		    {1.0f, 0.0f, 1.0f, 0.0f}};
		m_DefaultQuad = std::make_shared<VertexArray>(std::move(quad));
	}

	RenderTarget::~RenderTarget() {}

	void RenderTarget::Create(float x, float y, float width, float height) {
		m_Size = glm::vec2(width, height);
		m_View = glm::vec4(x, y, width, height);
		m_Projection = glm::ortho(x, x + width, y + height, y, -1.0f, 1.0f);
	}

	void RenderTarget::Draw(RenderState &state) {
		RTEAssert(state.m_Shader.get(), "Trying to render without a shader.");
		state.m_Shader->Use();
		state.m_BlendMode.Enable();
		if (state.m_Vertices) {
			state.m_Vertices->Bind();
		} else {
			m_DefaultQuad->Bind();
			state.m_PrimitiveType = PrimitiveType::TriangleStrip;
			state.m_Vertices = m_DefaultQuad;
		}

		if (state.m_Texture && state.m_Shader->GetTextureUniform() != -1) {
			state.m_Texture->Bind();
			state.m_Shader->SetInt(state.m_Shader->GetTextureUniform(), 0);
		}
		int transformUniform = state.m_Shader->GetTransformUniform();
		if (transformUniform != -1) {
			state.m_Shader->SetMatrix4f(transformUniform, state.m_ModelTransform);
		}
		int projectionUniform = state.m_Shader->GetProjectionUniform();
		if (projectionUniform != -1) {
			state.m_Shader->SetMatrix4f(projectionUniform, m_Projection);
		}
		int colorUniform = state.m_Shader->GetColorUniform();
		if (colorUniform != -1) {
			state.m_Shader->SetVector4f(colorUniform, state.m_Color);
		}

		// std::cout << state.m_ModelTransform[0].x << " " << state.m_ModelTransform[1].x << " " << state.m_ModelTransform[2].x << " " << state.m_ModelTransform[3].x << "\n"
		// 					<< state.m_ModelTransform[0].y << " " << state.m_ModelTransform[1].y << " " << state.m_ModelTransform[2].y << " " << state.m_ModelTransform[3].y << "\n"
		// 					<< state.m_ModelTransform[0].z << " " << state.m_ModelTransform[1].z << " " << state.m_ModelTransform[2].z << " " << state.m_ModelTransform[3].z << "\n"
		// 					<< state.m_ModelTransform[0].w << " " << state.m_ModelTransform[1].w << " " << state.m_ModelTransform[2].w << " " << state.m_ModelTransform[3].w << std::endl;

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, state.m_FBO));

		glCheck(glDrawArrays(static_cast<GLenum>(state.m_PrimitiveType), 0, state.m_Vertices->GetVertexCount()));
		glCheck(glBindVertexArray(0));

		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void RenderTarget::DrawClear(glm::vec4 color) {
		glCheck(glClearColor(color.r, color.g, color.b, color.a));
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		glCheck(glClear(GL_COLOR_BUFFER_BIT));
	}

	void RenderTarget::SetSize(glm::vec2 size) {
		m_Size = size;
		m_View.z = size.x;
		m_View.w = size.y;

		m_Projection = glm::ortho(m_View.x, m_View.x + m_View.z, m_View.y + m_View.w, m_View.y, -1.0f, 1.0f);
	}

	void RenderTarget::SetWrapXY(bool wrapX, bool wrapY) {
		m_WrapX = wrapX;
		m_WrapY = wrapY;
	}

} // namespace RTE
