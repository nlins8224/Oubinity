#pragma once
#include <glad/glad.h>
#include <array>

class SkyLoader {
 public:
  void load();
  void bindVAO();
  void bindEBO();
  GLuint getId();

 private:
  GLuint m_vao, m_vbo, m_ebo;
};