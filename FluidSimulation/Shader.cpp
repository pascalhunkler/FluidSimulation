#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::stringstream vertexShaderStream;
	std::stringstream fragmentShaderStream;
	
	try
	{
		// read vertex shader file
		std::ifstream vertexShaderFile(vertexShaderPath);
		vertexShaderStream << vertexShaderFile.rdbuf();
		vertexShaderFile.close();

		// read fragment shader file
		std::ifstream fragmentShaderFile(fragmentShaderPath);
		fragmentShaderStream << fragmentShaderFile.rdbuf();
		fragmentShaderFile.close();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	std::string vertexShaderSourceString = vertexShaderStream.str();
	std::string fragmentShaderSourceString = fragmentShaderStream.str();
	const char* vertexShaderSource = vertexShaderSourceString.c_str();
	const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

	int success;
	char infoLog[512];

	// Load and compile vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Load and compile fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Create program and link the shader
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
	}

	// Delete the vertex shader and the fragment shader, as they are not needed anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() const
{
	glUseProgram(id);
}

Shader::~Shader()
{
	glDeleteProgram(id);
}
