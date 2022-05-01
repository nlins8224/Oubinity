#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int p_screen_width, int p_screen_height, std::string p_screen_title);
	~Window();

	void windowInit();
	void processInput();
	GLFWwindow* getWindow();

private:
	GLFWwindow* m_window{};
	std::string m_screen_title{};
	int m_screen_width{}, m_screen_height{};
};