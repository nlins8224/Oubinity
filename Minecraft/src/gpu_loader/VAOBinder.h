#pragma once
#include <glad/glad.h>


class VAOBinder
{
public:
	void bindVAO(GLuint vao);
	void unbindVAO();
};