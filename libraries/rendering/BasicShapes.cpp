#include "BasicShapes.h"

#include <cmath>
#include <iostream>
#include "Model.h"
#include "Buffer.h"
#include "Shader.h"

static std::string shaderPath("C:/Users/dante/code/rendering-examples/resources/shaders/");
static const std::string vertexShader = shaderPath + "simple.vs";
static const std::string fragmentShader = shaderPath + "simple.fs";

static float const PI = 3.1415926f;
static float const RADIUS = 1.0f;

 static float const SECTOR_COUNT = 72.0f;
static float const STACK_COUNT = 24.0f;

inline float calculateStackAngle(int step)
{
    return (PI / 2) - (PI * (step / STACK_COUNT));
}

inline float calculateSectorAngle(int step)
{
    return (2.0f * PI) * (step / SECTOR_COUNT);
}
std::shared_ptr<Geometry> buildSphere()
{

    static float const LENGTH_INV = 1.0f / RADIUS;

    Mesh mesh;
    for (int i = 0; i <= STACK_COUNT; ++i)
    {
        float stackAngle = calculateStackAngle(i);
        float xy = RADIUS * std::cosf(stackAngle);
        float z = RADIUS * std::sinf(stackAngle);

        for (int j = 0; j <= SECTOR_COUNT; ++j)
        {
            float sectorAngle = calculateSectorAngle(j);
            float x = xy * std::cosf(sectorAngle);
            float y = xy * std::sinf(sectorAngle);

            float nx = x * LENGTH_INV;
            float ny = y * LENGTH_INV;
            float nz = z * LENGTH_INV;

            Vertex vertex({ x, y, z }, { nx, ny, nz });
            mesh.vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < STACK_COUNT; ++i)
    {
        int k1 = i * (SECTOR_COUNT + 1);
        int k2 = k1 + SECTOR_COUNT + 1;

        for (int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                mesh.indices.push_back(k1);
                mesh.indices.push_back(k2);
                mesh.indices.push_back(k1 + 1);
            }

            if (i != (STACK_COUNT -1))
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
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f))
    };


    mesh.indices = {
        0, 1, 2,
        2, 1, 3,
        0, 1, 4,
        1, 4, 5,
        4, 6, 5,
        5, 6, 7,
        0, 4, 2,
        2, 4, 6,
        2, 6, 3,
        3, 6, 7,
        1, 5, 3,
        5, 3, 2
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
