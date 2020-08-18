#include "BasicShapes.h"

#include <cmath>
#include <iostream>
#include "Model.h"
#include "Shader.h"
#include "Format.h"

static float const  PI = 3.14159265359;

static int const X_SEGMENTS = 256.0f;
static int const Y_SEGMENTS = 256.0f;
Model::Pointer buildSphere()
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



    Model::Pointer geometry = std::make_shared<Model>();
    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offset(Vertex, normal));

    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.vertexBuffer->setLayout(layout);
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);
    return geometry;
}


Model::Pointer buildTriangle()
{
    Model::Pointer geometry = std::make_shared<Model>();

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

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offset(Vertex, normal));
    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.vertexBuffer->setLayout(layout);
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);

    return geometry;
}


Model::Pointer buildCube()
{
    Model::Pointer geometry = std::make_shared<Model>();


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


    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offset(Vertex, normal));
    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    geometry->meshes.push_back(mesh);

    return geometry;
}

BasicShapes::BasicShapes()
{
    m_shapes[BasicShapes::CUBE] = buildCube();
    m_shapes[BasicShapes::TRIANGLE] = buildTriangle();
    m_shapes[BasicShapes::SPHERE] = buildSphere();
}



Model::Pointer BasicShapes::getShape(int type)
{
    return m_shapes[type];
}
