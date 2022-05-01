#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

typedef struct
{
	std::string vertex;
	std::string fragment;
} shader_code;

class Shader
{
public:
	Shader(std::string vertex_path, std::string fragment_path);
	unsigned int getID();
	void setUniformBool(const std::string& name, bool value) const;
	void setUniformInt(const std::string& name, int value) const;
	void setUniformFloat(const std::string& name, float value) const;
	void setUniformMat4(const std::string& name, const glm::mat4& mat) const;
	void useProgram();
private:
	unsigned int m_id;

	shader_code parseShader(std::string vertex_path, std::string fragment_path);
	unsigned int compileShader(int type, const std::string code);
	void createShaderProgram(unsigned int vertex_shader, unsigned int fragment_shader);
	void checkCompileErrors(unsigned int id, int program_type);
	void clean();

};