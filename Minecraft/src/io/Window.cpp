#include "Window.h"

const float Window::SCREEN_WIDTH = 1600.0f;
const float Window::SCREEN_HEIGHT = 1200.0f;

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

    m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_WIDTH, m_screen_title.c_str(), NULL, NULL);
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

Window::Window(std::string screen_title)
    : m_screen_title{ screen_title } {}

Window::~Window()
{
    glfwTerminate();
}


GLFWwindow* Window::getWindow()
{
    return m_window;
}
