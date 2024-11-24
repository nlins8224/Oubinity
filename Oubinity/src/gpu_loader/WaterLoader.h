#pragma once
#include <glad/glad.h>
#include <array>
#include "../water/WaterMesh.h"

class WaterLoader {
 public:
  void load();
  void bindVAO();
  GLuint getId();

 private:
  GLuint m_vao, m_vbo;
};