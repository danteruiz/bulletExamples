#include "BasicShapes.h"

#include <cmath>
#include <iostream>
#include "Model.h"
#include "Buffer.h"
#include "Shader.h"

static std::string shaderPath("C:/Users/dante/code/rendering-examples/resources/shaders/");
static const std::string vertexShader = shaderPath + "simple.vs";
static const std::string fragmentShader = shaderPath + "simple.fs";

static float const  PI = 3.14159265359;

static int const X_SEGMENTS = 256.0f;
static int const Y_SEGMENTS = 256.0f;
std::shared_ptr<Geometry> buildSphere()
{
    Mesh mesh;

    for (int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;

            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            Vertex vertex({xPos, yPos, zPos}, {xPos, yPos, zPos});
            mesh.vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < Y_SEGMENTS; ++i)
    {
        int k1 = i * (X_SEGMENTS + 1);
        int k2 = k1 + X_SEGMENTS + 1;

        for (int j = 0; j < X_SEGMENTS; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                mesh.indices.push_back(k1);
                mesh.indices.push_back(k2);
                mesh.indices.push_back(k1 + 1);
            }

            if (i != (Y_SEGMENTS -1))
            {
                mesh.indices.push_back(k1 + 1);
                mesh.indices.push_back(k2);
                mesh.indices.push_back(k2 +1);
            }
        }
    }



    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();

    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);
    geometry->shader = std::make_shared<Shader>(fragmentShader, vertexShader);
    return geometry;
}


std::shared_ptr<Geometry> buildTriangle()
{
    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();

    Mesh mesh;
    mesh.vertices = {
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),
    };

    mesh.indices = {
        0, 1, 2,
        0, 2, 4,
        0, 3, 4,
        0, 1, 3,
        1, 2, 3,
        2, 3, 4
    };

    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);
    geometry->shader = std::make_shared<Shader>(fragmentShader, vertexShader);

    return geometry;
}


std::shared_ptr<Geometry> buildCube()
{
    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();


    Mesh mesh;
    mesh.vertices = {
        // right side 0 - 3
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),

        //top side 4 - 7
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),

        //bottom side 8 - 11
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),

        // left 12 - 15
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),

        // front 16 - 19
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0, -1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0, -1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0, -1.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0, -1.0f)),

        //back 20 - 23
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    };


    mesh.indices = {
        // right side
        0, 1, 2,
        2, 1, 3,
        //top side
        4, 5 , 6,
        4, 6, 7,
        // bottom
        8, 9, 10,
        8, 10, 11,
        //left
        12, 13, 14,
        12, 14, 15,
        //front
        16, 17, 18,
        16, 18, 19,
        //back
        20, 21, 22,
        20, 22, 23
    };

    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);
    geometry->shader = std::make_shared<Shader>(fragmentShader, vertexShader);

    return geometry;
}

BasicShapes::BasicShapes()
{
    m_shapes[BasicShapes::CUBE] = buildCube();
    m_shapes[BasicShapes::TRIANGLE] = buildTriangle();
    m_shapes[BasicShapes::SPHERE] = buildSphere();
}



std::shared_ptr<Geometry> BasicShapes::getShape(int type)
{
    return m_shapes[type];
}
