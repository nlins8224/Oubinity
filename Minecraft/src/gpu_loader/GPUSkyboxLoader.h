#pragma once
#include <glad/glad.h>
#include <array>
#include "VAOBinder.h"

class GPUSkyboxLoader
{
public:
	void load();
	void bindVAO();
	GLuint getID();
private:
	GLuint m_vao, m_vbo;
	VAOBinder m_vao_binder;
};