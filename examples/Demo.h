#pragma once

#include "GlfwApplication.h"
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Window;
class Buffer;
class BasicShapes;
struct Geometry;




struct Transform
{
    glm::vec3 translation { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };
};

struct Entity
{
    glm::vec3 translation { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };

    glm::vec4 color { 1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<Geometry> geometry { nullptr };
};


struct Light
{
    float intensity { 1.0f };
    glm::vec3 color { 1.0f, 1.0f, 1.0f};
    glm::vec3 position { 1.0f, 1.0f, 1.0f };
};

class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();

    void exec() override;

private:
    Entity m_cubeEntity;
    Entity m_sphereEntity;
    Entity m_floorEntity;
    Entity m_triangleEntity;
    Light m_light;
    std::shared_ptr<BasicShapes> m_basicShapes;
};
