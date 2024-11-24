#pragma once
#include "../Camera.h"
#include "../shader/Shader.h"

class Renderer {
 public:
  Renderer(Shader shader);
  ~Renderer() = default;
  virtual void render(Camera& camera) = 0;
  Shader& getShader();

 protected:
  Shader m_shader;
};
