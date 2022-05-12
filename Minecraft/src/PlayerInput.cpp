#include "PlayerInput.h"
#include <iostream>


PlayerInput::PlayerInput(GLFWwindow* window, Camera default_camera): m_window{window}, m_default_camera{default_camera}
{
    setWindowCallbacks();
}

void PlayerInput::setWindowCallbacks()
{
    glfwSetKeyCallback(m_window, Keyboard::keyStateCallback);
    glfwSetMouseButtonCallback(m_window, Mouse::mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, Mouse::cursorPosCallback);
    glfwSetScrollCallback(m_window, Mouse::mouseWheelCallback);
}

Camera& PlayerInput::getCamera()
{
    return m_default_camera;
}

void PlayerInput::processInput(float delta_time)
{
    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window, true);

    if (Keyboard::key(GLFW_KEY_W))
        m_default_camera.updateCameraPos(FORWARD, delta_time);
    if (Keyboard::key(GLFW_KEY_S))
        m_default_camera.updateCameraPos(BACKWARD, delta_time);
    if (Keyboard::key(GLFW_KEY_A))
        m_default_camera.updateCameraPos(LEFT, delta_time);
    if (Keyboard::key(GLFW_KEY_D))
        m_default_camera.updateCameraPos(RIGHT, delta_time);
    if (Keyboard::key(GLFW_KEY_E))
        m_default_camera.updateCameraPos(UP, delta_time);
    if (Keyboard::key(GLFW_KEY_Q))
        m_default_camera.updateCameraPos(DOWN, delta_time);
}

