#pragma once
#include <string>

class Shader
{
public:
	// id of the shader
	unsigned int id = -1;

	/**
	 *	Create and link a new shader program
	 *	@param vertexShaderPath path to the vertex shader source
	 *	@param fragmentShaderPath path to the fragment shader source
	 */
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

	// activate/use the shader program
	void use() const;

	// delete the shader program
	~Shader();
};

