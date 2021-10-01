#ifndef _RTE_GLSLPROGRAM_
#define _RTE_GLSLPROGRAM_
#include "GL/glew.h"
namespace RTE {
	class GLSLProgram {
	public:
		GLSLProgram();
		~GLSLProgram();

		void CompileShaders(const std::string & vertexShaderFilepath, const std::string &fragmentShaderFilepath);

		void AddAttribute(const std::string &attributeName);

		void LinkShaders();

	private:
		GLuint m_VtxShaderID;
		GLuint m_FragShaderID;
		GLuint m_ProgramID;
		int m_NumAttrs;

		void CompileShader(const std::string &filename, GLuint shaderID);
	};
}
#endif
