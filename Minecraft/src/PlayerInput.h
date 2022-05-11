#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Keyboard.h"
#include "Mouse.h"

class PlayerInput
{
public:
	PlayerInput(GLFWwindow* window);
	void processInput(float delta_time);

private:
	GLFWwindow* m_window;
	void setWindowCallbacks();
};


