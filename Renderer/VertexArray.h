#ifndef _RTEVERTEXARRAY_
#define _RTEVERTEXARRAY_
namespace RTE {
	class Vertex;
	/// <summary>
	/// RAII wrapper for OpenGL vertex buffer and array objects.
	/// </summary>
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		VertexArray(const std::vector<Vertex>& vertices, std::optional<std::reference_wrapper<const std::vector<int>>> indices = std::nullopt);

		void Bind();

		unsigned long GetVAO() { return m_VAO; }

	private:
		friend class RenderTarget;
		unsigned long m_VAO;
		unsigned long m_VBO;
		unsigned long m_EBO;
		std::vector<Vertex> m_vertices;
	};
}
#endif
