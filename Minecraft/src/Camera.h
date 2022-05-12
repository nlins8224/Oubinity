#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraDirection {
    NONE,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:

    Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 getViewMatrix();
    float getZoom();
    void updateCameraDirection(double dx, double dy);
    void updateCameraPos(CameraDirection direction, double dt);
    void updateCameraZoom(double dy);

private:
    glm::vec3 m_camera_pos;
    glm::vec3 m_camera_front;
    glm::vec3 m_camera_up;
    glm::vec3 m_camera_right;
    glm::vec3 m_world_up;

    float m_yaw;
    float m_pitch;

    float m_speed;
    float m_zoom;

    void updateCameraVectors();
};