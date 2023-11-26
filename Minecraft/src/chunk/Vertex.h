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

000 000 000 000 00000 00000 00000 00000
            fff ttttt zzzzz yyyyy xxxxx
*/

struct Vertex
{
	uint32_t packed_vertex;
};

static inline GLuint packVertex(GLubyte x, GLubyte y, GLubyte z, GLubyte texture_id, GLubyte face_id)
{
	return x | y << 5 | z << 10 | texture_id << 15 | face_id << 20;
} 