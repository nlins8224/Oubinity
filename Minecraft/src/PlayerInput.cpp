#include "PlayerInput.h"
#include <iostream>

PlayerInput::PlayerInput(GLFWwindow* window): m_window{window}
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

void PlayerInput::processInput(float delta_time)
{
    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window, true);

    if (Keyboard::key(GLFW_KEY_W))
    {
        std::cout << "W: " << delta_time << '\n';
        Camera::m_default_camera.updateCameraPos(FORWARD, delta_time);
    }

    if (Keyboard::key(GLFW_KEY_S))
        Camera::m_default_camera.updateCameraPos(BACKWARD, delta_time);
    if (Keyboard::key(GLFW_KEY_A))
        Camera::m_default_camera.updateCameraPos(LEFT, delta_time);
    if (Keyboard::key(GLFW_KEY_D))
        Camera::m_default_camera.updateCameraPos(RIGHT, delta_time);
    if (Keyboard::key(GLFW_KEY_E))
        Camera::m_default_camera.updateCameraPos(UP, delta_time);
    if (Keyboard::key(GLFW_KEY_Q))
        Camera::m_default_camera.updateCameraPos(DOWN, delta_time);
}

