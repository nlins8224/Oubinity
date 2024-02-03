#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

/*
x - 5 bits
y - 5 bits
z - 5 bits
texture_id - 5 bits (t)
face_id - 3 bits (f)
vertex_id - 3 bits (v)

0 00 00 00 00 000 00000 00000 00000 00000
  aa aa	aa aa fff ttttt zzzzz yyyyy xxxxx
*/

struct Face
{
	GLuint packed_face;
};

static inline GLuint packFace(GLubyte x, GLubyte y, GLubyte z, GLubyte texture_id, GLubyte face_id, GLubyte a0, GLubyte a1, GLubyte a2, GLubyte a3)
{
	return x | y << 5 | z << 10 | texture_id << 15 | face_id << 20 | a0 << 23 | a1 << 25 | a2 << 27 | a3 << 29;
}