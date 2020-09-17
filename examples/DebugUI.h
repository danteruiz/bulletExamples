#pragma once

#include <memory>
#include <string>
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
    void show(std::vector<Entity> &entites, Light &light, std::function<void()> compileShader, std::function<void(std::string)> loadNewModel);
    bool focus();
    std::string getModelPath() const { return m_path; }

private:
    int m_entityIndex { 0 };
    float m_lightColor[3];
    bool m_focus { false };
    std::string m_path;
};
