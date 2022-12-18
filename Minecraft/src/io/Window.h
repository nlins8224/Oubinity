#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(std::string screen_title);
	~Window();

	void windowInit();
	void processInput();
	GLFWwindow* getWindow();
	static const float SCREEN_WIDTH, SCREEN_HEIGHT;

private:
	GLFWwindow* m_window{};
	std::string m_screen_title{};
};