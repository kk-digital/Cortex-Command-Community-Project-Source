#include "RenderTarget.h"

#include "System/RTEError.h"

#include "Shader.h"
#include "Vertex.h"
#include "GLTexture.h"
#include "GL/glew.h"

namespace RTE {
	RenderTarget::RenderTarget() {
		std::vector<Vertex> quad {
			{0.0f, 0.0f, 0.0f, 0.0f},
		};
	}

	RenderTarget::~RenderTarget() {}

	void RenderTarget::draw(RenderState &&state) {
		RTEAssert(state.m_Shader.get(), "Trying to render without a shader.");
		state.m_Shader->Use();
		state.m_BlendMode.Enable();
		if (state.m_Vertices) {
			state.m_Vertices->Bind();
		} else {
			m_DefaultQuad.Bind();
		}
    }
}
