#pragma once

#include <memory>
#include <vector>

struct Entity;
struct Light;
class Window;
class DebugUI
{
public:
    DebugUI(std::shared_ptr<Window> const &window);
    ~DebugUI();
    void show(std::vector<Entity> &entites, Light &light);

private:
    int m_entityIndex { 0 };
    float m_lightColor[3];
};
