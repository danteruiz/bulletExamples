#include "GlfwApplication.h"

#include "Glfw.h"
#include <GLFW/glfw3.h>

#include "Window.h"


static GlfwApplication* g_application { nullptr };
GlfwApplication::GlfwApplication() {

    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (!glfw::initialize())
    {
        std::cout << "THROW ERRORS: FAILED TO INITIALIZE GLFW" << std::endl;
    }
    std::cout << "GlfwApplication::GlfwApplication" << std::endl;
    if (g_application) {
        std::cout << "!!!! ERROR: INSTANCE ALREADY EXISTS" << std::endl;
    }

    g_application = this;
    m_string = "This is not a test";


    m_window = std::make_shared<Window>(500, 500, "Demo");
    m_window->createWindow();
}


GlfwApplication* GlfwApplication::instance()
{
    return g_application;
}


GlfwApplication::~GlfwApplication()
{
    g_application = nullptr;
}
