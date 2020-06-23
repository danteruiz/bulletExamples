#pragma once

#include <memory>
#include <vector>
#include <functional>

struct Entity;
struct Light;
class Window;
class DebugUI
{
public:
    DebugUI(std::shared_ptr<Window> const &window);
    ~DebugUI();
    void show(std::vector<Entity> &entites, Light &light, std::function<void()> compileShader);
    bool focus();

private:
    int m_entityIndex { 0 };
    float m_lightColor[3];
    bool m_focus { false };
};
