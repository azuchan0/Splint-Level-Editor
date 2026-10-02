#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void bind() const;
	void unbind() const;
	void setData(const std::vector<glm::vec2>& vertices);
	void draw(GLenum mode) const;

private:
	GLuint m_VAO;
	GLuint m_VBO;
	size_t m_vertexCount;
};