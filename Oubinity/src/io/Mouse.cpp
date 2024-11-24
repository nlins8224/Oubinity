#include "Mouse.h"

double Mouse::m_x = 0;
double Mouse::m_y = 0;

double Mouse::m_last_x = 0;
double Mouse::m_last_y = 0;

double Mouse::m_dx = 0;
double Mouse::m_dy = 0;

double Mouse::m_scroll_dx = 0;
double Mouse::m_scroll_dy = 0;

double Mouse::m_sensitivity = 0.7f;

bool Mouse::m_first_mouse = true;

bool Mouse::m_buttons[GLFW_MOUSE_BUTTON_LAST] = {0};
bool Mouse::m_buttons_changed[GLFW_MOUSE_BUTTON_LAST] = {0};

void Mouse::cursorPosCallback(GLFWwindow* window, double x, double y) {
  PlayerInput* ptr =
      static_cast<PlayerInput*>(glfwGetWindowUserPointer(window));

  m_x = x;
  m_y = y;

  if (m_first_mouse) {
    m_last_x = m_x;
    m_last_y = m_y;
    m_first_mouse = false;
  }

  m_dx = m_x - m_last_x;
  m_dy = m_last_y - m_y;  // inverted

  m_last_x = m_x;
  m_last_y = m_y;

  ptr->getCamera().updateCameraDirection(m_dx * m_sensitivity,
                                         m_dy * m_sensitivity);
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action,
                                int mods) {
  if (action != GLFW_RELEASE) {
    if (!m_buttons[button]) {
      m_buttons[button] = true;
    }
  } else {
    m_buttons[button] = false;
  }

  m_buttons_changed[button] = action != GLFW_REPEAT;
}

void Mouse::mouseWheelCallback(GLFWwindow* window, double dx, double dy) {
  PlayerInput* ptr =
      static_cast<PlayerInput*>(glfwGetWindowUserPointer(window));
  ptr->getCamera().updateCameraZoom(dy);
}

double Mouse::getMouseX() { return m_x; }

double Mouse::getMouseY() { return m_y; }

double Mouse::getDX() {
  double dx = m_dx;
  m_dx = 0;
  return dx;
}

double Mouse::getDY() {
  double dy = m_dy;
  m_dy = 0;
  return dy;
}

double Mouse::getScrollDX() {
  double dx = m_scroll_dx;
  m_scroll_dx = 0;
  return dx;
}

double Mouse::getScrollDY() {
  double dy = m_scroll_dy;
  m_scroll_dy = 0;
  return dy;
}

bool Mouse::button(int button) { return m_buttons[button]; }

bool Mouse::buttonChanged(int button) {
  bool ret = m_buttons_changed[button];
  m_buttons_changed[button] = false;
  return ret;
}

bool Mouse::buttonWentUp(int button) {
  return !m_buttons[button] && buttonChanged(button);
}

bool Mouse::buttonWentDown(int button) {
  return m_buttons[button] && buttonChanged(button);
}
