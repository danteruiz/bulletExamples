#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

class Buffer;
class Shader;

struct Vertex
{
    glm::vec3 position;
};


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
};



struct Model
{
    std::vector<Mesh> meshes;
    std::shared_ptr<Shader> shader;
};

// void loadModel(std::string const &filePath);
