#include "GPUSkyboxLoader.h"

void GPUSkyboxLoader::load()
{
	glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Skybox::skybox_vertices), Skybox::skybox_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void GPUSkyboxLoader::bindVAO() const
{
	m_vao_binder.bindVAO(m_vao);
}

GLuint GPUSkyboxLoader::getID()
{
    return m_vao;
}