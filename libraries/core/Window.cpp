#include "Window.h"


#include "Glfw.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Window::Window(int width, int height, std::string title)
{
    m_width = width;
    m_height = height;
    m_windowTitle = title;
}

Window::~Window()
{
    glfw::destroyWindow(m_glfwWindow);
    glfw::terminate();
}


void Window::simpleUpdate()
{
    glfw::pollEvents();
}

void Window::swap() { glfw::swapBuffers(m_glfwWindow); }

//Init the window and check for any window errors
bool Window::createWindow()
{

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    m_glfwWindow = glfw::createWindow(m_width, m_height, m_windowTitle);

    if (!m_glfwWindow)
    {
        glfw::terminate();
        return false;
    }

    glfwMakeContextCurrent(m_glfwWindow);

    return true;
}


bool Window::shouldClose()
{
    return glfw::windowShouldClose(m_glfwWindow);
}


void Window::setWidthAndHeight(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}
