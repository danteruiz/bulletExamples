#pragma once

#include "Window.h"

#include <memory>
#include <iostream>
#include <string>

class GlfwApplication
{
public:
    GlfwApplication();
    ~GlfwApplication();
    virtual void exec() {}


    static GlfwApplication* instance();

    void print() { std::cout << m_string << std::endl; }


private:
    std::shared_ptr<Window> m_activeWindow();

    std::string m_string { "Hello" };
};
