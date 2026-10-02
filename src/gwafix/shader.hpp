#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void bind() const;
	void unbind() const;

	void setMat4(const std::string& name, glm::mat4& mat) const;
	void setVec3(const std::string& name, glm::vec3& vec) const;

private:
	GLuint m_RendererID;
	void checkCompileError(GLuint shader, const std::string& type) const;
};