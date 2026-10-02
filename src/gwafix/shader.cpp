#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "erreur::shader::file_not_read" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	
	
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileError(vertex, "vertex");

	
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileError(fragment, "fragment");

	
	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, vertex);
	glAttachShader(m_RendererID, fragment);
	glLinkProgram(m_RendererID);
	checkCompileError(m_RendererID, "program");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {
	glDeleteProgram(m_RendererID);
}

void Shader::bind() const {
	glUseProgram(m_RendererID);
}

void Shader::unbind() const {
	glUseProgram(0);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
	glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::checkCompileError(GLuint shader, const std::string& type) const {
	GLint success;
	GLchar infoLog[1024];
	if (type != "program") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "error::shader_compilation::" << type << "\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "error::program_linking::" << type << "\n" << infoLog << std::endl;
		}
	}
}

