#pragma once
#include <glad/glad.h>
#include <array>
#include "../skybox/SkyboxMesh.h"
#include "VAOBinder.h"

class SkyboxLoader {
 public:
  void load();
  void bindVAO() const;
  GLuint getID();

 private:
  GLuint m_vao, m_vbo;
  VAOBinder m_vao_binder;
};