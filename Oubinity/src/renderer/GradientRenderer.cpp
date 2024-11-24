#include "GradientRenderer.h"

GradientRenderer::GradientRenderer(Shader shader) : Renderer(shader) {}

void GradientRenderer::render(Camera& camera) {
  glDepthFunc(GL_LEQUAL);
  m_shader.bind();

  // remove translation from view matrix by casting it out
  glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
  glm::mat4 projection = camera.getProjectionMatrix();
  m_shader.setUniformMat4("view", view);
  m_shader.setUniformMat4("projection", projection);

  m_gradient_loader.bindVAO();
  draw();
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}

void GradientRenderer::draw() {
  m_shader.setUniformFloat("u_size", Skybox::SIZE);
  const GLuint gradient_triangles_amount = Skybox::vertices_amount / 3;
  glDrawArrays(GL_TRIANGLES, 0, gradient_triangles_amount);
}

SkyboxLoader& GradientRenderer::getGradientLoader() {
  return m_gradient_loader;
}
