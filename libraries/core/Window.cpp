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

#ifdef __APPLE__
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    m_glfwWindow = glfw::createWindow(m_width, m_height, m_windowTitle);

    if (!m_glfwWindow)
    {
        glfw::terminate();
        return false;
    }

    glfwMakeContextCurrent(m_glfwWindow);


    if (GL_ARB_shading_language_include)
    {
        std::cout << "can include" << std::endl;
    }

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
