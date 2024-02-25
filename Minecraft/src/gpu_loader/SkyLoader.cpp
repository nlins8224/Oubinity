#include "SkyLoader.h"

/* Does not store anything in the buffer, because sky is raymarched */
void SkyLoader::load()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void SkyLoader::bindVAO()
{
	glBindVertexArray(m_vao);
}

GLuint SkyLoader::getId()
{
	return m_vao;
}