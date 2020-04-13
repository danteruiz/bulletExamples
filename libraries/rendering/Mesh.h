#pragma once

#include <vector>
#include <glm/glm.hpp>


namespace render
{
    struct Vertex
    {
        glm::vec3 position;
    };



    class Buffer;
    class Mesh
    {
    public:
        Mesh() = default;
        ~Mesh() = default;

        std::vector<Vertex> m_vertices;
        std::vector<int> m_indices;


        // Buffer m_vertexBuffer;
        //Buffer m_indexBuffer;
    };
}
