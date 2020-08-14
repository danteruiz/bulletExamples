#pragma once
#include "Buffer.h"

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

struct Vertex
{
    Vertex(glm::vec3 p) : position(p) {}
    Vertex(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 uv) : position(p), normal(n), texCoord(uv) {}
    glm::vec3 position;
    glm::vec3 normal { 0.0f, 1.0f, 0.0f };
    glm::vec2 texCoord { 0.0f, 0.0f };
};


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    Buffer::Pointer vertexBuffer;
    Buffer::Pointer indexBuffer;
    Material::Pointer material { nullptr };
};



struct Model
{
    using Pointer = std::shared_ptr<Model>;
    std::vector<Mesh> meshes;
};


Model::Pointer loadModel(std::string const &file);
