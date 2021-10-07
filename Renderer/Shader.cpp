#include "Shader.h"
#include <GL/glew.h>
#include "glm/gtc/type_ptr.hpp"

namespace RTE {

	Shader::Shader() :
	    m_ProgramID(glCreateProgram()) {}

	Shader::~Shader() {
		if (m_ProgramID) {
			glDeleteProgram(m_ProgramID);
		}
	}

	bool Shader::Compile(const std::string &vertexPath, const std::string &fragPath) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		bool result{false};

		std::string error;
		result = compileShader(vertexShader, vertexPath, error) && compileShader(fragmentShader, fragPath, error);
		if (result)
			Link(vertexShader, fragmentShader);
		else {
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		return true;
	}

	void Shader::Use() { glUseProgram(m_ProgramID); }

	void Shader::SetBool(const std::string &name, bool value) { glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), static_cast<int>(value)); }

	void Shader::SetInt(const std::string &name, int value) { glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value); }

	void Shader::SetFloat(const std::string &name, float value) { glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value); }

	void Shader::SetMatrix4f(const std::string &name, const glm::mat4 &value) { glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

	void Shader::SetVector2f(const std::string &name, const glm::vec2 &value) { glUniform2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value)); }

	void Shader::SetVector3f(const std::string &name, const glm::vec3 &value) { glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value)); }

	void Shader::SetVector4f(const std::string &name, const glm::vec4 &value) { glUniform4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value)); }

	bool Shader::compileShader(GLuint shaderID, const std::string &filename, std::string &error) {
		std::ifstream file(filename);
		std::ostringstream dataStream;

		dataStream << file.rdbuf();

		std::string dataString = dataStream.str();

		const char *data = dataString.c_str();

		glShaderSource(shaderID, 1, &data, nullptr);
		glCompileShader(shaderID);
		GLint success;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE) {
			GLint infoLength;
			error += "\nFailed to compile " + filename + ":\n";
			size_t errorPrevLen = error.size();
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLength);
			error.resize(errorPrevLen + infoLength);
			glGetShaderInfoLog(shaderID, infoLength, &infoLength, error.data() + errorPrevLen);
			return false;
		}

		return true;
	}

	bool Shader::Link(GLuint vtxShader, GLuint fragShader) {
		glAttachShader(m_ProgramID, vtxShader);
		glAttachShader(m_ProgramID, fragShader);

		glLinkProgram(m_ProgramID);

		glDetachShader(m_ProgramID, vtxShader);
		glDetachShader(m_ProgramID, fragShader);
		glDeleteShader(vtxShader);
		glDeleteShader(fragShader);

		return true;
	}
} // namespace RTE
