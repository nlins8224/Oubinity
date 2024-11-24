#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

class Window {
 public:
  Window(std::string screen_title);
  ~Window();

  void windowInit();
  void processInput();
  GLFWwindow* getWindow();
  static const float SCREEN_WIDTH, SCREEN_HEIGHT;

 private:
  GLFWwindow* m_window{};
  std::string m_screen_title{};
};