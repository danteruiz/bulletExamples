#pragma once

#include <memory>
#include <iostream>
#include <string>


class Window;
class GlfwApplication
{
public:
    GlfwApplication();
    ~GlfwApplication();

    virtual void exec() = 0;

    std::shared_ptr<Window> getWindow() { return m_window; }
    static GlfwApplication* instance();
protected:
    std::shared_ptr<Window> m_window { nullptr };

    std::string m_string { "Hello" };
};
