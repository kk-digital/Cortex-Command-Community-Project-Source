#include "VertexArray.h"
#include "Vertex.h"
#include "GLCheck.h"

#include "GL/glew.h"

namespace RTE {
	VertexArray::VertexArray() {
		glCheck(glGenVertexArrays(1, &m_VAO));
		glCheck(glGenBuffers(1, &m_VBO));
	}

	VertexArray::~VertexArray() {
		if (m_VBO) {
			glCheck(glDeleteBuffers(1, &m_VBO));
		}

		if (m_VAO) {
			glCheck(glDeleteVertexArrays(1, &m_VAO));
		}
	}

	VertexArray::VertexArray(const std::vector<Vertex> &vertices, bool updateable) :
	    VertexArray() {
		Create(vertices, updateable);
	}

	void VertexArray::Create(const std::vector<Vertex> &vertices, bool updateable) {
		assert(m_VBO != 0);
		m_Vertices = vertices;
		glCheck(glBindVertexArray(m_VAO));
		if (!vertices.empty()) {
			glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
			glCheck(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), updateable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
			glCheck(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, pos))));
			glCheck(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texUV))));
			glCheck(glVertexAttribPointer(2, 4, GL_UNSIGNED_INT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, color))));
		}
		glCheck(glBindVertexArray(0));
	}

	void VertexArray::Bind() {
		glCheck(glBindVertexArray(m_VAO));
	}

	size_t VertexArray::GetVertexCount() { return m_Vertices.size(); }

	void VertexArray::AddVertex(Vertex &&vertex) {
		m_Vertices.emplace_back(vertex);
		m_VerticesUpdated = true;
	}

	void VertexArray::InsertVertex(int index, Vertex &&vertex) {
		m_Vertices.emplace(m_Vertices.begin() + index, vertex);
		m_VerticesUpdated = true;
	}

	void VertexArray::Reserve(size_t n) {
		m_Vertices.reserve(n);
	}

	void VertexArray::ResetVertices() {
		m_Vertices.clear();
		m_VerticesUpdated = true;
	}
	void VertexArray::Update() {
		if (m_VerticesUpdated) {
			if (!m_Vertices.empty()) {
				glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
				glCheck(glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_DYNAMIC_DRAW));

				glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
			} else {
				glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
				glCheck(glMapBufferRange(GL_ARRAY_BUFFER, 0, 0, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
				glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
			}
		}
	}
} // namespace RTE
