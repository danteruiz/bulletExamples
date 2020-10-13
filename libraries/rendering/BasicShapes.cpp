#include "BasicShapes.h"

#include <cmath>
#include <iostream>
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Format.h"

static float const  PI = 3.14159265359;

static int const X_SEGMENTS = 64.0f;
static int const Y_SEGMENTS = 64.0f;
Model::Pointer buildSphere()
{
    Model::Pointer geometry = std::make_shared<Model>();

    auto& vertices = geometry->vertices;
    auto& indices = geometry->indices;
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
            vertices.push_back(vertex);
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
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (Y_SEGMENTS -1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 +1);
            }
        }
    }

    Primitive primitive;
    primitive.indexStart = 0;
    primitive.vertexStart = 0;
    primitive.vertexCount = static_cast<uint32_t>(vertices.size());
    primitive.indexCount =  static_cast<uint32_t>(indices.size());
    primitive.materialName = "default";

    mesh.primitives.push_back(primitive);

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, geometry->vertices.size() * sizeof(Vertex), geometry->vertices.size(), geometry->vertices.data());
    geometry->vertexBuffer->setLayout(layout);
    geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, geometry->indices.size() * sizeof(int), geometry->indices.size(), geometry->indices.data());

    //geometry->materials["default"] = std::make_shared<Material>();

    geometry->meshes.push_back(mesh);
    geometry->hasIndexBuffer = true;
    return geometry;
}


Model::Pointer buildQuad()
{
    Model::Pointer model  = std::make_shared<Model>();

    Mesh mesh;

    model->vertices = {
        Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(1.0, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f))
    };

    model->indices = {
        0, 1, 2,
        1, 2, 3
    };

    Primitive primitive;
    primitive.indexStart = 0;
    primitive.vertexStart = 0;
    primitive.vertexCount = static_cast<uint32_t>(model->vertices.size());
    primitive.indexCount =  static_cast<uint32_t>(model->indices.size());
    primitive.materialName = "default";

    mesh.primitives.push_back(primitive);

     std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex), (unsigned int) offsetof(Vertex,texCoord));

    model->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, model->vertices.size() * sizeof(Vertex), model->vertices.size(), model->vertices.data());
    model->vertexBuffer->setLayout(layout);
    model->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, model->indices.size() * sizeof(int), model->indices.size(), model->indices.data());

    model->meshes.push_back(mesh);

    return model;
}


Model::Pointer buildTriangle()
{
    Model::Pointer geometry = std::make_shared<Model>();

    Mesh mesh;
    geometry->vertices = {
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),
    };

    geometry->indices = {
        0, 1, 2,
        0, 2, 4,
        0, 3, 4,
        0, 1, 3,
        1, 2, 3,
        2, 3, 4
    };

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, geometry->vertices.size() * sizeof(Vertex), geometry->vertices.size(), geometry->vertices.data());
    geometry->vertexBuffer->setLayout(layout);
    geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, geometry->indices.size() * sizeof(int), geometry->indices.size(), geometry->indices.data());
    geometry->meshes.push_back(mesh);

    return geometry;
}


Model::Pointer buildCube()
{
    Model::Pointer geometry = std::make_shared<Model>();


    Mesh mesh;
    geometry->vertices = {
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


    geometry->indices = {
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
        13, 14, 15,
        //front
        16, 17, 18,
        16, 18, 19,
        //back
        20, 21, 22,
        20, 22, 23
    };


    Primitive primitive;
    primitive.indexStart = 0;
    primitive.vertexStart = 0;
    primitive.vertexCount = static_cast<uint32_t>(geometry->vertices.size());
    primitive.indexCount =  static_cast<uint32_t>(geometry->indices.size());

    mesh.primitives.push_back(primitive);

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex), (unsigned int) offsetof(Vertex,texCoord));

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, geometry->vertices.size() * sizeof(Vertex), geometry->vertices.size(), geometry->vertices.data());
    geometry->vertexBuffer->setLayout(layout);
    geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, geometry->indices.size() * sizeof(int), geometry->indices.size(), geometry->indices.data());
    geometry->meshes.push_back(mesh);

    return geometry;
}

BasicShapes::BasicShapes()
{
    m_shapes[BasicShapes::CUBE] = buildCube();
    m_shapes[BasicShapes::TRIANGLE] = buildTriangle();
    m_shapes[BasicShapes::SPHERE] = buildSphere();
    m_shapes[BasicShapes::QUAD] = buildQuad();
}



Model::Pointer BasicShapes::getShape(int type)
{
    return m_shapes[type];
}
