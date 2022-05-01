#include "Shader.h"

Shader::Shader(std::string vertex_path, std::string fragment_path)
{
	shader_code code = parseShader(vertex_path, fragment_path);
	unsigned int vertex_shader = compileShader(GL_VERTEX_SHADER, code.vertex);
	unsigned int fragment_shader = compileShader(GL_FRAGMENT_SHADER, code.fragment);
	createShaderProgram(vertex_shader, fragment_shader);
	clean();
}

unsigned int Shader::getID()
{
	return m_id;
}

void Shader::useProgram()
{
	glUseProgram(m_id);
}

void Shader::setUniformBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<int>(value));
}

void Shader::setUniformInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

shader_code Shader::parseShader(std::string vertex_path, std::string fragment_path)
{
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// possibly should be c_str()
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;

		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();

		v_shader_file.close();
		f_shader_file.close();

		return shader_code{ v_shader_stream.str(), f_shader_stream.str() };
	}
	catch (std::ifstream::failure& e)
	{
		std::cerr << "Error: Shader file not succesfully read: " << e.what() << std::endl;
		return shader_code{ "", "" };
	}
}

unsigned int Shader::compileShader(int type, const std::string code)
{
	//This cstr_code might be uneccessary
	const char* cstr_code = code.c_str();
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &cstr_code, NULL);
	glCompileShader(shader);
	checkCompileErrors(shader, type);
	return shader;
}

void Shader::createShaderProgram(unsigned int vertex_shader, unsigned int fragment_shader)
{
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);
	glLinkProgram(m_id);

	checkCompileErrors(m_id, GL_PROGRAM);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::checkCompileErrors(unsigned int shader_id, int program_type)
{
	int success;
	char infoLog[1024];

	if (GL_VERTEX_SHADER == program_type)
	{
		glGetProgramiv(shader_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			std::cout << success << std::endl;
			glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
			std::cerr << "ERROR::VERTEX_SHADER_COMPILATION_ERROR: " << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}

	}
	else if (GL_FRAGMENT_SHADER == program_type)
	{
		glGetProgramiv(shader_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
			std::cerr << "ERROR::FRAGMENT_SHADER_COMPILATION_ERROR: " << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else if (GL_PROGRAM == program_type)
	{
		glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader_id, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void Shader::clean()
{

}
