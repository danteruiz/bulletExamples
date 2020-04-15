#include "GlfwApplication.h"


static GlfwApplication* g_application { nullptr };
GlfwApplication::GlfwApplication() {

    if (g_application) {
        std::cout << "!!!! ERROR: INSTANCE ALREADY EXISTS" << std::endl;
    }

    g_application = this;
    m_string = "This is not a test";
}


GlfwApplication* GlfwApplication::instance()
{
    return g_application;
}


GlfwApplication::~GlfwApplication()
{
    g_application = nullptr;
}
