#pragma once

#include "GlfwApplication.h"
#include <memory>
#include <vector>

#include "Entity.h"


#include <Skybox.h>



class Window;
class Buffer;
class BasicShapes;
class DebugUI;
struct Geometry;
class Shader;

struct Marker
{
    glm::vec3 position { 0.0f, 0.0f, 0.0f };
    glm::quat orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };
};

class DebugDraw
{
public:
    DebugDraw();
    void renderMarkers(std::vector<Marker> const &markers, glm::mat4 const &view, glm::mat4 const &projection);
private:
    std::shared_ptr<Buffer> m_vertexBuffer { nullptr };
    std::shared_ptr<Shader> m_debugPipeline { nullptr };
};


class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();
    void exec() override;
private:
    Light m_light;
    Entity m_modelEntity;
    Skybox m_skybox;


    std::shared_ptr<BasicShapes> m_basicShapes;
    std::shared_ptr<DebugUI> m_debugUI;
    std::vector<Light> m_lights;
    std::shared_ptr<Shader> m_pipeline { nullptr };
    std::shared_ptr<Shader> m_irradiance { nullptr };
    std::shared_ptr<Shader> m_convertToCubeMap { nullptr };
    std::shared_ptr<Shader> m_filterMap { nullptr };
    std::shared_ptr<Shader> m_brdfLut { nullptr };
    std::shared_ptr<DebugDraw> m_debugDraw { nullptr };
    std::vector<Marker> m_markers;
};
