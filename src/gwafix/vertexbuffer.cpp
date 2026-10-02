#include "vertexbuffer.hpp"

VertexBuffer::VertexBuffer() : m_vertexCount(0) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void VertexBuffer::bind() const {
	glBindVertexArray(m_VAO);
}

void VertexBuffer::unbind() const {
	glBindVertexArray(0);
}

void VertexBuffer::setData(const std::vector<glm::vec2>& vertices) {
	m_vertexCount = vertices.size(); 
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);
}

void VertexBuffer::draw(GLenum mode) const {
	if (m_vertexCount == 0) return;
	bind();
	glDrawArrays(mode, 0, m_vertexCount);
	unbind();
}








