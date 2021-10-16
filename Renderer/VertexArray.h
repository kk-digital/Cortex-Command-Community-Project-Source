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

		VertexArray(const std::vector<Vertex>& vertices);

		void Bind();

		unsigned long GetVAO() { return m_VAO; }

		size_t GetVertexCount() { return m_Vertices.size();}

	private:
		friend class RenderTarget;
		unsigned int m_VAO;
		unsigned int m_VBO;
		std::vector<Vertex> m_Vertices;
	};
}
#endif
