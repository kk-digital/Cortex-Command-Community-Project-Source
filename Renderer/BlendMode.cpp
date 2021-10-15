#include "BlendMode.h"
#include "GL/glew.h"

namespace RTE {
	void BlendMode::Enable() const {
		if(!glIsEnabled(GL_BLEND))
			glEnable(GL_BLEND);
		GLenum srcBlendFunc;
		GLenum dstBlendFunc;
		switch (m_SrcBlendFunc) {
			case BlendFunc::Add: {
				srcBlendFunc = GL_FUNC_ADD;
			} break;
			case BlendFunc::Subtract: {
				srcBlendFunc = GL_FUNC_SUBTRACT;
			} break;
			case BlendFunc::ReverseSubtract: {
				srcBlendFunc = GL_FUNC_REVERSE_SUBTRACT;
			} break;
		}
		switch (m_DestBlendFunc) {
			case BlendFunc::Add: {
				dstBlendFunc = GL_FUNC_ADD;
			} break;
			case BlendFunc::Subtract: {
				dstBlendFunc = GL_FUNC_SUBTRACT;
			} break;
			case BlendFunc::ReverseSubtract: {
				dstBlendFunc = GL_FUNC_REVERSE_SUBTRACT;
			} break;
		}

		glBlendEquationSeparate(srcBlendFunc, dstBlendFunc);
		glBlendFuncSeparate(static_cast<GLenum>(m_SrcColor), static_cast<GLenum>(m_DestColor), static_cast<GLenum>(m_SrcAlpha), static_cast<GLenum>(m_DestAlpha));
	}
} // namespace RTE
