#include "Camera.h"

Camera::Camera(glm::vec3 position)
    : m_camera_pos(position),
    m_world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_yaw(-90.0f),
    m_pitch(0.0f),
    m_speed(2.5f),
    m_camera_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_fov(90.0f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, m_camera_up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return glm::perspective(glm::radians(m_fov), Window::SCREEN_WIDTH /  Window::SCREEN_HEIGHT, 0.1f, 200.0f);
}

float Camera::getZoom()
{
    return m_zoom;
}

void Camera::updateCameraDirection(double dx, double dy)
{
    m_yaw += dx;
    m_pitch += dy;

    if (m_pitch > 89.0f)
    {
        m_pitch = 89.0f;
    }
    else if (m_pitch < -89.0f)
    {
        m_pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraPos(CameraDirection direction, double dt)
{
    float velocity = static_cast<float>(dt) * m_speed;

    switch (direction)
    {
    case CameraDirection::FORWARD:
        m_camera_pos += m_camera_front * velocity;
        break;
    case CameraDirection::BACKWARD:
        m_camera_pos -= m_camera_front * velocity;
        break;
    case CameraDirection::RIGHT:
        m_camera_pos += m_camera_right * velocity;
        break;
    case CameraDirection::LEFT:
        m_camera_pos -= m_camera_right * velocity;
        break;
    case CameraDirection::UP:
        m_camera_pos += m_world_up * velocity;
        break;
    case CameraDirection::DOWN:
        m_camera_pos -= m_world_up * velocity;
        break;
    }
}

void Camera::updateCameraZoom(double dy)
{
    if (m_zoom >= 1.0f && m_zoom <= 45.0f)
    {
        m_zoom -= dy;
    }
    else if (m_zoom < 1.0f)
    {
        m_zoom = 1.0f;
    }
    else
    {
        m_zoom = 45.0f;
    }
}

glm::vec3 Camera::getCameraPos()
{
    return m_camera_pos;
}

glm::vec3 Camera::getCameraDirection()
{
    return m_camera_front;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front_direction;
    front_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front_direction.y = sin(glm::radians(m_pitch));
    front_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera_front = glm::normalize(front_direction);
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_camera_right = glm::normalize(glm::cross(m_camera_front, m_world_up));  
    m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));
}
