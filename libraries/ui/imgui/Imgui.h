#pragma once

#include <vector>

#include <string>
#include <vector>

struct GLFWwindow;
namespace imgui
{
    void initialize(GLFWwindow *window);
    void uninitialize();
    void render();
    void newFrame();
    bool ListBox(std::string name, int* index, std::vector<std::string> &items);
}
