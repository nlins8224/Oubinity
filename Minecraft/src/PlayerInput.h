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
	// TODO: ChunkManager should not be in this constructor
	PlayerInput(GLFWwindow* window, ChunkManager& world, Camera& default_camera);
	void processInput(float delta_time);
	Camera& getCamera();

private:
	Camera& m_default_camera;
	GLFWwindow* m_window;
	ChunkManager& m_world;
	void setWindowCallbacks();
	void onMouseLeftPress();
	void onMouseRightPress();
};


