#pragma once

#include <iostream>

struct GLFWwindow;
class Window
{
public:
    ~Window();

    bool createWindow();
    bool shouldClose();

    void simpleUpdate();
    void swap();
    Window(int width, int height, std::string title);


    int getHeight() const { return m_height; }
    int getWidth() const { return m_width; }

    void setHeight(int height) { m_height = height; }
    void setWidth(int width) { m_width = width; }

    GLFWwindow* getWindowPtr() const { return m_glfwWindow; }
private:
    GLFWwindow* m_glfwWindow { nullptr };
    std::string m_windowTitle { "" };

    int m_height { 50 };
    int m_width { 50 };

};
