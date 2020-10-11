#pragma once
#include "Buffer.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <inttypes.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


struct Material;
class Shader;
struct Vertex
{
    Vertex(glm::vec3 p) : position(p) {}
    Vertex(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 uv) : position(p), normal(n), texCoord(uv) {}
    glm::vec3 position;
    glm::vec3 normal { 0.0f, 1.0f, 0.0f };
    glm::vec2 texCoord { 0.0f, 0.0f };
};



struct Primitive
{
    uint32_t indexStart;
    uint32_t vertexStart;
    uint32_t indexCount;
    uint32_t vertexCount;
    std::string materialName;
};

struct Mesh
{
    std::vector<Primitive> primitives;
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    Buffer::Pointer vertexBuffer;
    Buffer::Pointer indexBuffer;
    glm::mat4 matrix;
};



glm::mat4 getLocalMeshMatrix(Mesh const &mesh);


struct Model
{
    using Pointer = std::shared_ptr<Model>;
    std::vector<Mesh> meshes;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Buffer::Pointer vertexBuffer;
    Buffer::Pointer indexBuffer;
    bool hasIndexBuffer { false };
    std::unordered_map<std::string, std::tuple<std::shared_ptr<Material>, std::shared_ptr<Shader>>> materials;
};

Model::Pointer loadModel(std::string const &file);
