#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "../PlayerInput.h"

class Mouse {
 public:
  static void cursorPosCallback(GLFWwindow* window, double x, double y);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);
  static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

  double getMouseX();
  double getMouseY();

  double getDX();
  double getDY();

  double getScrollDX();
  double getScrollDY();

  static bool button(int button);
  static bool buttonChanged(int button);
  static bool buttonWentUp(int button);
  static bool buttonWentDown(int button);

 private:
  static double m_x;
  static double m_y;

  static double m_last_x;
  static double m_last_y;

  static double m_dx;
  static double m_dy;

  static double m_scroll_dx;
  static double m_scroll_dy;

  static bool m_first_mouse;
  static double m_sensitivity;

  static bool m_buttons[];
  static bool m_buttons_changed[];
};