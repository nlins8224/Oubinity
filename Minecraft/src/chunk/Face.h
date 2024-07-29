#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Face
{
	GLuint packed_face_one;
	GLuint packed_face_two;
};


static inline GLuint packFaceOne(GLubyte x, GLubyte y, GLubyte z, GLubyte w, GLubyte h)
{
	return x | y << 6 | z << 12 | w << 18 | h << 24;
}

static inline GLuint packFaceTwo(GLubyte face_id, GLubyte texture_id)
{
	return face_id | texture_id << 3;
}