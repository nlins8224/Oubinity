#include "Window.h"

void Window::processInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

void Window::windowInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_screen_width, m_screen_width, m_screen_title.c_str(), NULL, NULL);
    if (m_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window);
    /* Lambda is needed because C++ method cannot be passed as C function to GLFW API */
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* p_window, int p_screen_width, int p_screen_height) {
        glViewport(0, 0, p_screen_width, p_screen_height);
        });
}

Window::Window(int p_screen_width, int p_screen_height, std::string p_screen_title)
    : m_screen_width{ p_screen_width },
    m_screen_height{ p_screen_height },
    m_screen_title{ p_screen_title } {}

Window::~Window()
{
    glfwTerminate();
}


GLFWwindow* Window::getWindow()
{
    return m_window;
}
