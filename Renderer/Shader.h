#ifndef _RTE_SHADER_
#define _RTE_SHADER_
namespace RTE {
	class Shader {
	public:
		Shader();
		~Shader();

		bool Compile(const std::string &vertexFilename, const std::string &fragPath);

		void Use();

		void SetBool(const std::string &name, bool value);
		void SetInt(const std::string &name, int value);
		void SetFloat(const std::string &name, float value);
		void SetMatrix4f(const std::string &name, const glm::mat4 &value);
		void SetVector2f(const std::string &name, const glm::vec2 &value);
		void SetVector3f(const std::string &name, const glm::vec3 &value);
		void SetVector4f(const std::string &name, const glm::vec4 &value);

	private:
		uint32_t m_ProgramID;

		bool compileShader(uint32_t shaderID, const std::string &data, std::string &error);
		bool Link(uint32_t vtxShader, uint32_t fragShader);
	};
} // namespace RTE
#endif
