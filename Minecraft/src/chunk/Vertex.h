#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

/*
x - 5 bits
y - 5 bits
z - 5 bits
texture_id - 5 bits (t)
face_id - 3 bits (f)
shading_value - 3 bits (s)
vertex_id - 3 bits (v)

000 000 000 000 00000 00000 00000 00000
    vvv sss fff ttttt zzzzz yyyyy xxxxx
*/

struct Vertex
{
	uint32_t packed_vertex;
};

static inline GLuint packVertex(GLubyte x, GLubyte y, GLubyte z, GLubyte texture_id, GLubyte face_id, GLubyte shading_value, GLubyte vertex_id)
{
	return x | y << 5 | z << 10 | texture_id << 15 | face_id << 20 | shading_value << 23 | vertex_id << 26;
}