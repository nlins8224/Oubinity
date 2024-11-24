#pragma once
#include "../gpu_loader/SkyboxLoader.h"
#include "../shader/SkyboxShader.h"
#include "Renderer.h"

class SkyboxRenderer : public Renderer {
 public:
  SkyboxRenderer() = delete;
  SkyboxRenderer(Shader shader, GLuint skybox_texture_id);
  ~SkyboxRenderer() = default;
  void render(Camera& camera);
  void draw();
  SkyboxLoader& getSkyboxLoader();

 private:
  SkyboxLoader m_skybox_loader;
  GLuint m_skybox_texture_id;
  bool is_loaded{false};
};