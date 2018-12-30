#pragma once

#include <glm/glm.hpp>


namespace render
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoord;
    };



    class Buffer;
    class Mesh
    {
        Mesh() = default;
        ~Mesh() = default;
    };
}
