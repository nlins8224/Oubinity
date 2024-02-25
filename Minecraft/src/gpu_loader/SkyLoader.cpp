#include "SkyLoader.h"



/* Does not store anything in the buffer, because sky is raymarched */
void SkyLoader::load()
{
	float vertices[] = {
		// positions   // texture coords
		 1.0f,  1.0f, 1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 1.0f, 0.0f,   // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f,   // bottom left
		-1.0f,  1.0f, 0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

void SkyLoader::bindVAO()
{
	glBindVertexArray(m_vao);
}

void SkyLoader::bindEBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

GLuint SkyLoader::getId()
{
	return m_vao;
}