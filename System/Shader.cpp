#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"

#include "Managers/ConsoleMan.h"
#include "System.h"
#include "RTEError.h"
#include "glad/gl.h"

namespace RTE {

	const std::string Shader::c_ClassName = "Shader";

	Shader::Shader() :
	    m_ProgramID(glCreateProgram()), m_TextureUniform(-1), m_ColorUniform(-1), m_TransformUniform(-1), m_ProjectionUniform(-1) {}

	Shader::Shader(const std::string &vertexFilename, const std::string &fragPath) :
	    m_ProgramID(glCreateProgram()), m_TextureUniform(-1), m_ColorUniform(-1), m_TransformUniform(-1), m_ProjectionUniform(-1) {
		Create(vertexFilename, fragPath);
	}

	Shader::~Shader() {
		if (m_ProgramID) {
			(glDeleteProgram(m_ProgramID));
		}
	}

	int Shader::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "VertexShader") {
			reader >> m_VertexFile;
		} else if (propName == "FragmentShader") {
			reader >> m_FragmentFile;
		} else {
			return Serializable::ReadProperty(propName, reader);
		}
		return 0;
	}

	int Shader::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewPropertyWithValue("VertexShader", m_VertexFile);
		writer.NewPropertyWithValue("FragmentShader", m_FragmentFile);

		return 0;
	}

	int Shader::Create() {
		return !Compile(m_VertexFile, m_FragmentFile);
	}

	int Shader::Create(const std::string &vertexFilename, const std::string &fragPath) {
		m_VertexFile = vertexFilename;
		m_FragmentFile = fragPath;

		return Create();
	}

	bool Shader::Compile(const std::string &vertexPath, const std::string &fragPath) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		bool result{false};

		std::string error;
		result = CompileShader(vertexShader, vertexPath, error) && CompileShader(fragmentShader, fragPath, error);
		if (result) {
			(glBindAttribLocation(m_ProgramID, 0, "rteVertexPosition"));
			(glBindAttribLocation(m_ProgramID, 1, "rteVertexTexUV"));
			(glBindAttribLocation(m_ProgramID, 2, "rteVertexColor"));
			if (Link(vertexShader, fragmentShader)) {
				m_TextureUniform = GetUniformLocation("rteTexture");
				m_ColorUniform = GetUniformLocation("rteColor");
				m_TransformUniform = GetUniformLocation("rteTransform");
				m_UVTransformUniform = GetUniformLocation("rteUVTransform");
				m_ProjectionUniform = GetUniformLocation("rteProjection");
			} else {
			}
		} else {
			(glDeleteShader(vertexShader));
			(glDeleteShader(fragmentShader));

			RTEAbort("ERROR: Failed to compile shaders:\n" + error);
			return false;
		}

		ApplyDefaultUniforms();

		return true;
	}

	void Shader::Use() { (glUseProgram(m_ProgramID)); }

	GLint Shader::GetUniformLocation(const std::string &name) { return glGetUniformLocation(m_ProgramID, name.c_str()); }

	void Shader::SetBool(const std::string &name, bool value) { glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), static_cast<int>(value)); }

	void Shader::SetInt(const std::string &name, int value) { glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value); }

	void Shader::SetFloat(const std::string &name, float value) { glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value); }

	void Shader::SetMatrix4f(const std::string &name, const glm::mat4 &value) { glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

	void Shader::SetVector2f(const std::string &name, const glm::vec2 &value) { glUniform2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value)); }

	void Shader::SetVector3f(const std::string &name, const glm::vec3 &value) { glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value)); }

	void Shader::SetVector4f(const std::string &name, const glm::vec4 &value) { (glUniform4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value))); }

	void Shader::SetBool(int32_t uniformLoc, bool value) { (glUniform1i(uniformLoc, value)); }

	void Shader::SetInt(int32_t uniformLoc, int value) { (glUniform1i(uniformLoc, value)); }

	void Shader::SetFloat(int32_t uniformLoc, float value) { (glUniform1f(uniformLoc, value)); }

	void Shader::SetMatrix4f(int32_t uniformLoc, const glm::mat4 &value) { (glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value))); }

	void Shader::SetNMatrix(int32_t uniformLoc, size_t n, const GLfloat *values) { glUniformMatrix4fv(uniformLoc, n, GL_FALSE, values); }

	void Shader::SetVector2f(int32_t uniformLoc, const glm::vec2 &value) { (glUniform2fv(uniformLoc, 1, glm::value_ptr(value))); }

	void Shader::SetVector3f(int32_t uniformLoc, const glm::vec3 &value) { (glUniform3fv(uniformLoc, 1, glm::value_ptr(value))); }

	void Shader::SetVector4f(int32_t uniformLoc, const glm::vec4 &value) { (glUniform4fv(uniformLoc, 1, glm::value_ptr(value))); }

	bool Shader::CompileShader(GLuint shaderID, const std::string &filename, std::string &error) {
		if (!System::PathExistsCaseSensitive(filename)) {
			error += "File " + filename + " doesn't exist.";
			return false;
		}

		std::ifstream file(filename);
		std::ostringstream dataStream;

		dataStream << file.rdbuf();

		std::string dataString = dataStream.str();

		const char *data = dataString.c_str();

		(glShaderSource(shaderID, 1, &data, nullptr));
		(glCompileShader(shaderID));
		GLint success;
		(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));

		if (success == GL_FALSE) {
			GLint infoLength;
			error += "\nFailed to compile " + filename + ":\n";
			size_t errorPrevLen = error.size();
			(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLength));
			error.resize(errorPrevLen + infoLength);
			(glGetShaderInfoLog(shaderID, infoLength, &infoLength, error.data() + errorPrevLen));
			return false;
		}

		return true;
	}

	bool Shader::Link(GLuint vtxShader, GLuint fragShader) {
		(glAttachShader(m_ProgramID, vtxShader));
		(glAttachShader(m_ProgramID, fragShader));

		(glLinkProgram(m_ProgramID));

		(glDetachShader(m_ProgramID, vtxShader));
		(glDetachShader(m_ProgramID, fragShader));
		(glDeleteShader(vtxShader));
		(glDeleteShader(fragShader));

		return true;
	}

	void Shader::ApplyDefaultUniforms() {
		Use();
		SetInt("rteTexture", 0);
		SetInt("rtePalette", 1);
		SetVector4f("rteColor", glm::vec4(1));
		SetMatrix4f("rteTransform", glm::mat4(1));
		SetMatrix4f("rteProjection", glm::mat4(1));
	}
} // namespace RTE
