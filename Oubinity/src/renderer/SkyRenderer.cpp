#include "SkyRenderer.h"

SkyRenderer::SkyRenderer(Shader shader, GLuint cloud_noise_texture_id,
                         glm::vec2 resolution)
    : Renderer(shader),
      m_cloud_noise_texture_id{cloud_noise_texture_id},
      m_resolution{resolution} {}

void SkyRenderer::render(Camera& camera) {
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  m_shader.bind();
  m_shader.setUniformVec2f("u_resolution", m_resolution);
  m_shader.setUniformFloat("u_time", m_time_elapsed_sec);
  glm::vec3 camera_front = camera.getCameraFront();
  glm::vec3 camera_right = camera.getCameraRight();
  glm::vec3 camera_up = camera.getCameraUp();

  float pitch = camera.getPitch();
  float yaw = camera.getYaw();

  m_shader.setUniformVec3f("u_camera_front", camera.getCameraFront());
  m_shader.setUniformVec3f("u_camera_right", camera.getCameraRight());
  m_shader.setUniformVec3f("u_camera_up", camera.getCameraUp());
  m_shader.setUniformVec3f("u_camera_pos", camera.getCameraPos());
  m_shader.setUniformVec2f("u_mouse", m_mouse_pos);
  m_shader.setUniformFloat("u_pitch", camera.getPitch());
  m_shader.setUniformFloat("u_yaw", camera.getYaw());

  m_sky_loader.bindVAO();
  // m_sky_loader.bindEBO();
  draw();
  glBindVertexArray(0);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
}

void SkyRenderer::draw() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_cloud_noise_texture_id);

  // glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SkyRenderer::setTimeElapsed(float time_in_sec) {
  m_time_elapsed_sec = time_in_sec;
}

void SkyRenderer::setMousePos(glm::vec2 mouse_pos) { m_mouse_pos = mouse_pos; }

SkyLoader& SkyRenderer::getSkyLoader() { return m_sky_loader; }
