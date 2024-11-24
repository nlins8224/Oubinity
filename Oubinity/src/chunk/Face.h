#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Face {
  GLuint packed_face_one;
  GLuint packed_face_two;
};

static inline GLuint packFaceOne(GLubyte x, GLubyte y, GLubyte z, GLubyte w,
                                 GLubyte h) {
  return x | y << 6 | z << 12 | w << 18 | h << 24;
}

static inline GLuint packFaceTwo(GLubyte face_id, GLubyte texture_id,
                                 GLubyte a0, GLubyte a1, GLubyte a2,
                                 GLubyte a3) {
  return face_id | texture_id << 3 | a0 << 8 | a1 << 10 | a2 << 12 | a3 << 14;
}