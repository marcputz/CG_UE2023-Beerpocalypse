#include "MyShader.h"

MyShader::MyShader() : ID_(0) {
	
}

void MyShader::compile(const char* vertexSourceCode, const char* fragmentSourceCode) {
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSourceCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSourceCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	ID_ = glCreateProgram();
	glAttachShader(ID_, vertex);
	glAttachShader(ID_, fragment);
	glLinkProgram(ID_);

	glGetProgramiv(ID_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID_, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void MyShader::use() {
	glUseProgram(ID_);
}

void MyShader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID_, name.c_str()), (int)value);
}

void MyShader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID_, name.c_str()), value);
}

void MyShader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID_, name.c_str()), value);
}

void MyShader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
}

void MyShader::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID_, name.c_str()), x, y);
}

void MyShader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
}

void MyShader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID_, name.c_str()), x, y, z);
}

void MyShader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
}

void MyShader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID_, name.c_str()), x, y, z, w);
}

void MyShader::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void MyShader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}

void MyShader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

}