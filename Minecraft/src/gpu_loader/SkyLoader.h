#pragma once
#include <glad/glad.h>
#include <array>

class SkyLoader
{
public:
	void load();
	void bindVAO();
	GLuint getId();
private:
	GLuint m_vao;
};