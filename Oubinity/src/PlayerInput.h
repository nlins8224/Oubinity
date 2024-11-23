#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

class PlayerInput
{
public:
	PlayerInput(GLFWwindow* window, Camera& default_camera);
	void processInput(float delta_time);
	Camera& getCamera();

private:
	Camera& m_default_camera;
	GLFWwindow* m_window;
	bool m_is_wireframe_enabled;
	void setWindowCallbacks();
	void toggleWireframeMode();
	void enableWireframeMode();
	void disableWireframeMode();
};


