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
    void show(Entity &entity, Light &light, std::function<void()> compileShader, std::function<void(std::string, bool)> loadNewModel);
    bool focus();
    std::string getModelPath() const { return m_path; }

private:
    float m_lightColor[3];
    bool m_focus { false };
    bool m_useModel { false };
    int m_modelCurrentIndex { 0 };
    int m_materialIndex { 0 };
    std::string m_path;
};
