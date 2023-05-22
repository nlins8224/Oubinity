#include "PlayerInput.h"
#include <iostream>


PlayerInput::PlayerInput(GLFWwindow* window, ChunkManager& world, Camera& default_camera)
    : m_window{window}, m_world{ world }, m_default_camera{default_camera}
{
    setWindowCallbacks();
    m_is_wireframe_enabled = false;
}

void PlayerInput::setWindowCallbacks()
{
    glfwSetKeyCallback(m_window, Keyboard::keyStateCallback);
    glfwSetMouseButtonCallback(m_window, Mouse::mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, Mouse::cursorPosCallback);
    glfwSetScrollCallback(m_window, Mouse::mouseWheelCallback);
}

void PlayerInput::onMouseLeftPress()
{
    //auto hit_callback = [this](glm::vec3 current_block, glm::vec3 next_block) {
    //    this->m_world.updateBlock(next_block, Block::AIR);
    //};

    //Ray hit_ray{ this->m_world, m_default_camera.getCameraPos(), m_default_camera.getCameraDirection() };
    //while (hit_ray.getDistance() < 10.0)
    //{
    //    if (hit_ray.step(hit_callback))
    //        break;
    //}
}

void PlayerInput::onMouseRightPress()
{
    //auto hit_callback = [&](glm::vec3 current_block, glm::vec3 next_block) mutable {
    //    m_world.updateBlock(current_block, Block::PLANKS);
    //};

    //Ray hit_ray{ m_world, m_default_camera.getCameraPos(), m_default_camera.getCameraDirection() };
    //while (hit_ray.getDistance() < 10.0)
    //{
    //    if (hit_ray.step(hit_callback))
    //        break;
    //}
}

void PlayerInput::toggleWireframeMode()
{
    if (m_is_wireframe_enabled)
        disableWireframeMode();
    else
        enableWireframeMode();
    std::cout << "toggleWireframeMode: " << m_is_wireframe_enabled << std::endl;
}

void PlayerInput::enableWireframeMode()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_is_wireframe_enabled = true;
}

void PlayerInput::disableWireframeMode()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_is_wireframe_enabled = false;
}


Camera& PlayerInput::getCamera()
{
    return m_default_camera;
}

void PlayerInput::processInput(float delta_time)
{
    float speed = 10 * delta_time;

    if (Keyboard::key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window, true);

    if (Keyboard::key(GLFW_KEY_W))
        m_default_camera.updateCameraPos(CameraDirection::FORWARD, speed);
    if (Keyboard::key(GLFW_KEY_S))
        m_default_camera.updateCameraPos(CameraDirection::BACKWARD, speed);
    if (Keyboard::key(GLFW_KEY_A))
        m_default_camera.updateCameraPos(CameraDirection::LEFT, speed);
    if (Keyboard::key(GLFW_KEY_D))
        m_default_camera.updateCameraPos(CameraDirection::RIGHT, speed);
    if (Keyboard::key(GLFW_KEY_E))
        m_default_camera.updateCameraPos(CameraDirection::UP, speed);
    if (Keyboard::key(GLFW_KEY_Q))
        m_default_camera.updateCameraPos(CameraDirection::DOWN, speed);
    if (Keyboard::keyWentUp(GLFW_KEY_F1))
        toggleWireframeMode();

    if (Mouse::button(GLFW_MOUSE_BUTTON_LEFT))
        onMouseLeftPress();
    if (Mouse::button(GLFW_MOUSE_BUTTON_RIGHT))
        onMouseRightPress();
}

