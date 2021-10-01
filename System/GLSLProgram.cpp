#include "GLSLProgram.h"
#include "Reader.h"

#include "RTEError.h"

namespace RTE{
	GLSLProgram::GLSLProgram() : m_VtxShaderID(0),m_FragShaderID(0),m_ProgramID(0) {}

	GLSLProgram::~GLSLProgram() {}

	void GLSLProgram::CompileShaders(const std::string &vertexShaderFilepath, const std::string &fragmentShaderFilepath) {
		m_ProgramID = glCreateProgram();

		m_VtxShaderID = glCreateShader(GL_VERTEX_SHADER);

		if(!m_VtxShaderID){
			RTEAbort("Failed to create vertex shader!");
		}

		m_FragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if(!m_FragShaderID) {
			glDeleteShader(m_VtxShaderID);
			RTEAbort("Failed to create fragment shader");
		}

		CompileShader(vertexShaderFilepath, m_VtxShaderID);
		CompileShader(fragmentShaderFilepath, m_FragShaderID);
	}

	void GLSLProgram::AddAttribute(const std::string &attributeName) {
		glBindAttribLocation(m_ProgramID, m_NumAttrs++, attributeName.c_str());
	}

	void GLSLProgram::LinkShaders() {

		glAttachShader(m_ProgramID, m_VtxShaderID);
		glAttachShader(m_ProgramID, m_FragShaderID);

		glLinkProgram(m_ProgramID);

		GLint isLinked = 0;
		glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &isLinked);

		if(isLinked == GL_FALSE){
			GLint maxLength = 0;
			glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);
			std::string errorLog(maxLength, '-');
			glGetProgramInfoLog(m_ProgramID, maxLength, &maxLength, errorLog.data());
			glDetachShader(m_ProgramID, m_VtxShaderID);
			glDetachShader(m_ProgramID, m_FragShaderID);
			glDeleteProgram(m_ProgramID);
			glDeleteShader(m_VtxShaderID);
			glDeleteShader(m_FragShaderID);

			RTEAbort("ERROR: Failed to link shaders because:\n" + errorLog);
		}

		glDetachShader(m_ProgramID, m_VtxShaderID);
		glDetachShader(m_ProgramID, m_FragShaderID);
		glDeleteShader(m_VtxShaderID);
		glDeleteShader(m_FragShaderID);
		m_VtxShaderID = 0;
		m_FragShaderID = 0;
	}

	void GLSLProgram::CompileShader(const std::string &filename, GLuint shaderID) {
		Reader reader(filename);
		std::istream* stream = reader.GetStream();

		std::ostringstream fileContents;
		fileContents << stream->rdbuf();
		std::string shaderCode = fileContents.str();
		const char *shaderCodeString = shaderCode.c_str();
		glShaderSource(shaderID, 1, &(shaderCodeString), nullptr);

		glCompileShader(shaderID);

		GLint success = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			std::string errorLog(maxLength, ' ');
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, errorLog.data());

			glDeleteShader(shaderID);

			RTEAbort("ERROR: Failed to compile shader from: " + filename +" because:\n"  + errorLog);
		}
	}
}
