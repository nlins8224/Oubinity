#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>

#include "Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"
#include "renderer/ChunkRenderer.h"

class PlayerInput {
 public:
  PlayerInput(GLFWwindow* window, Camera& default_camera, ChunkRenderer& chunk_renderer);
  void processInput(float delta_time);
  Camera& getCamera();

 private:
  Camera& m_default_camera;
  ChunkRenderer& m_world;
  GLFWwindow* m_window;
  bool m_is_wireframe_enabled;
  void onMouseLeftPress();
  void onMouseRightPress();
  void setWindowCallbacks();
  void toggleWireframeMode();
  void enableWireframeMode();
  void disableWireframeMode();
};
