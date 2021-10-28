#include "RenderTarget.h"

#include "System/RTEError.h"

#include "Shader.h"
#include "Vertex.h"
#include "GLTexture.h"

#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"


namespace RTE {
	RenderTarget::RenderTarget() {
		std::vector<Vertex> quad {
			{0.0f, 1.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f, 0.0f}
		};
		m_DefaultQuad = VertexArray(quad);
	}

	RenderTarget::~RenderTarget() {}

	void RenderTarget::Create(int x, int y, int width, int height) {
		m_Projection = glm::ortho(x, y, x + width, y + height);
	}


	void RenderTarget::Draw(RenderState &state) {
		RTEAssert(state.m_Shader.get(), "Trying to render without a shader.");
		state.m_Shader->Use();
		state.m_BlendMode.Enable();
		if (state.m_Vertices) {
			state.m_Vertices->Bind();
		} else {
			m_DefaultQuad.Bind();
			state.m_PrimitiveType = PrimitiveType::TriangleStrip;
		}

		if(state.m_Texture && state.m_Shader->GetTextureUniform() != -1){
			state.m_Texture->Bind();
			state.m_Shader->SetInt(state.m_Shader->GetTextureUniform(), 0);
		}
		int transformUniform = state.m_Shader->GetTransformUniform();
		if(transformUniform != -1) {
			state.m_Shader->SetMatrix4f(transformUniform, state.m_ModelTransform);
		}
		int projectionUniform = state.m_Shader->GetProjectionUniform();
		if(projectionUniform != -1) {
			state.m_Shader->SetMatrix4f(projectionUniform, m_Projection);
		}
		int colorUniform = state.m_Shader->GetColorUniform();
		if(colorUniform != -1) {
			state.m_Shader->SetVector4f(colorUniform, state.m_Color);
		}

		glDrawArrays(static_cast<GLenum>(state.m_PrimitiveType), 0, state.m_Vertices->GetVertexCount());
    }

	void RenderTarget::DrawClear() {
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
