#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "Camera.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Ray.h"

class PlayerInput
{
public:
	PlayerInput(GLFWwindow* window, ChunkManager& world, Camera default_camera = glm::vec3(0.0f, 0.0f, 3.0f));
	void processInput(float delta_time);
	Camera& getCamera();
	Camera m_default_camera{ glm::vec3(0.0f, 0.0f, 3.0f) };

private:
	GLFWwindow* m_window;
	ChunkManager& m_world;
	void setWindowCallbacks();
	void onMouseLeftPress();
	void onMouseRightPress();
};


