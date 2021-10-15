#include "VertexArray.h"
#include "Vertex.h"

#include "GL/glew.h"

namespace RTE {
	VertexArray::VertexArray() : m_VAO(0), m_VBO(0), m_EBO(0) {
		glCreateVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}

	VertexArray::~VertexArray() {
		if (m_EBO) {
			glDeleteBuffers(1, &m_EBO);
		}

		if (m_VBO) {
			glDeleteBuffers(1, &m_VBO);
		}

		if (m_VAO) {
			glDeleteVertexArrays(1, &m_VAO);
		}
	}

	VertexArray::VertexArray(const std::vector<Vertex>& vertices, std::optional<std::reference_wrapper<const std::vector<int>>> indices) : VertexArray() {
		glBindVertexArray(m_VAO);
		if(!vertices.empty()) {
			glBindBuffer(GL_VERTEX_ARRAY, m_VBO);
			glBufferData(GL_VERTEX_ARRAY, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texUV)));
			glVertexAttribPointer(2, 4, GL_UNSIGNED_INT, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
		}
		if(indices) {
			const std::vector<int>& indexList = indices->get();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexList.size() * sizeof(int), indexList.data(), GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}


	void VertexArray::Bind() {
		glBindVertexArray(m_VAO);
	}
}
