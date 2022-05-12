#pragma once

#include <GLAD/glad.h>
#pragma once
#include <GLFW/glfw3.h>
#include "PlayerInput.h"
#include <iostream>

class Mouse
{
public:

	static void cursorPosCallback(GLFWwindow* window, double x, double y);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

	double getMouseX();
	double getMouseY();

	double getDX();
	double getDY();

	double getScrollDX();
	double getScrollDY();

	bool button(int button);
	bool buttonChanged(int button);
	bool buttonWentUp(int button);
	bool buttonWentDown(int button);

private:
	static double m_x;
	static double m_y;

	static double m_last_x;
	static double m_last_y;

	static double m_dx;
	static double m_dy;

	static double m_scroll_dx;
	static double m_scroll_dy;

	static bool m_first_mouse;
	static double m_sensitivity;

	static bool m_buttons[];
	static bool m_buttons_changed[];
};