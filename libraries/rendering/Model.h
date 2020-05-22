#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

class Buffer;
class Shader;

struct Vertex
{
    Vertex(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}
    Vertex(glm::vec3 p) : position(p) {}
    glm::vec3 position;
    glm::vec3 normal { 0.0f, 1.0f, 0.0f };
};


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
};



struct Geometry
{
    std::vector<Mesh> meshes;
    std::shared_ptr<Shader> shader;
};
