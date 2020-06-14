#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Geometry;

struct Transform
{
    glm::vec3 translation { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };
};


struct Entity
{
    using EntityPointer = std::shared_ptr<Entity>;
    std::string name { "Entity" };
    glm::vec3 translation { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };

    glm::vec4 color  { 1.0f, 1.0f, 1.0f, 1.0f };
    float specular { 1.0f };
    float shininess { 32.0f };
    std::shared_ptr<Geometry> geometry { nullptr };
};


struct Light
{
    float intensity { 1.0f };
    float ambient { 1.0f };
    glm::vec3 color { 1.0f, 1.0f, 1.0f};
    glm::vec3 position { 1.0f, 1.0f, 1.0f };
};
