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

		VertexArray(const std::vector<Vertex>& vertices, bool updateable = false);

		void Create(const std::vector<Vertex>& vertices, bool updateable = false);

		void Bind();

		unsigned long GetVAO() { return m_VAO; }

		size_t GetVertexCount();

		void AddVertex(Vertex vertex);
		void InsertVertex(Vertex vertex, int index);
		void Reserve(size_t n);
		void ResetVertices();
		void Update();

	private:
		friend class RenderTarget;
		unsigned int m_VAO;
		unsigned int m_VBO;
		std::vector<Vertex> m_Vertices;
		bool m_StaticDraw;

		bool m_VerticesUpdated;
	};
}
#endif
