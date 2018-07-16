#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(int width, int height, std::string title)
{
    m_width = width;
    m_height = height;
    m_windowTitle = title;
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}


void Window::simpleUpdate()
{
    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();
}

//Init the window and check for any window errors
bool Window::createWindow()
{
    // TODO: should make this it own thing.
    if (!glfwInit())
    {
        return false;
    }

    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_windowTitle.c_str(), NULL, NULL);

    if (!m_glfwWindow)
    {
        glfwTerminate();
        return false;
    }

    return true;
}


bool Window::shouldClose()
{
    return glfwWindowShouldClose(m_glfwWindow);
}
