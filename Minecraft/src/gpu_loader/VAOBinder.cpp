#include "VAOBinder.h"

void VAOBinder::bindVAO(GLuint vao)
{
	glBindVertexArray(vao);
}

void VAOBinder::unbindVAO()
{
	glBindVertexArray(0);
}
