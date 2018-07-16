#pragma once

#include <iostream>

class GLFWwindow;
class Window
{
public:
    ~Window();

    bool createWindow();
    bool shouldClose();

    void simpleUpdate();
    Window(int width, int height, std::string title);

private:
    //    GLFWWindow* windowHandle;

    GLFWwindow* m_glfwWindow { nullptr };
    std::string m_windowTitle { "" };

    int m_height { 50 };
    int m_width { 50 };

};
