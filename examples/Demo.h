#pragma once

#include "GlfwApplication.h"
#include <memory>
#include <vector>

#include "Entity.h"



class Window;
class Buffer;
class BasicShapes;
class DebugUI;
struct Geometry;
class Shader;


class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();

    void exec() override;

private:

    std::vector<Entity> m_entities;
    Light m_light;
    std::shared_ptr<BasicShapes> m_basicShapes;
    std::shared_ptr<DebugUI> m_debugUI;

    std::shared_ptr<Shader> m_pipeline { nullptr };
};
