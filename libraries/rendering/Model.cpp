#include "Model.h"

#include "Buffer.h"
#include "Shader.h"
#include "Material.h"
#include "Format.h"
#include "Texture.h"

#define TINYGLTF_IMPLEMENTATION
#include "external/tiny_gltf.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <string>

std::vector<std::shared_ptr<Texture>> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string textureType)
{
    std::vector<std::shared_ptr<Texture>> textures;

    for (unsigned int index = 0; index < mat->GetTextureCount(type); ++index)
    {
        aiString str;
        mat->GetTexture(type, index, &str);
        std::string path = std::string(str.C_Str());
        auto texture = loadTexture(path);
        std::cout << path << std::endl;
        textures.push_back(texture);
    }


    return textures;
}

Mesh processMesh(tinygltf::Model &model, tinygltf::Mesh& gltfMesh)
{
    Mesh mesh;

    for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = gltfMesh.primitives[i];


        auto attributes = primitive.attributes;
        auto positionIndex = attributes["POSITION"];
        auto normalIndex = attributes["NORMAL"];
        auto texCoordIndex = attributes["TEXCOORD_0"];

        tinygltf::BufferView const &positionBufferView = model.bufferViews[positionIndex];
        tinygltf::BufferView const &normalBufferView = model.bufferViews[normalIndex];
        tinygltf::BufferView const &texCoordBufferView = model.bufferViews[texCoordIndex];

        const tinygltf::Accessor &positionAccess = model.accessors[positionIndex];
        const tinygltf::Accessor &normalAccess = model.accessors[normalIndex];
        const tinygltf::Accessor &texCoordAccess = model.accessors[texCoordIndex];

        tinygltf::Buffer &positionBuffer = model.buffers[positionBufferView.buffer];
        tinygltf::Buffer &normalBuffer = model.buffers[normalBufferView.buffer];
        tinygltf::Buffer &texCoordBuffer = model.buffers[texCoordBufferView.buffer];

        const float* positionData = reinterpret_cast<float*> (&positionBuffer.data[positionBufferView.byteOffset + positionAccess.byteOffset]);
        const float* normalData = reinterpret_cast<float*> (&normalBuffer.data[normalBufferView.byteOffset + normalAccess.byteOffset]);
        const float* texCoordData = reinterpret_cast<float*> (&texCoordBuffer.data[texCoordBufferView.byteOffset + texCoordAccess.byteOffset]);


        for (size_t i = 0; i < positionAccess.count; ++i)
        {

            size_t posOffset = i *  3;
            glm::vec3 pos(positionData[posOffset + 0], positionData[posOffset + 1], positionData[posOffset + 2]);

            size_t normOffset = i * 3;
            glm::vec3 norm(normalData[normOffset + 0], normalData[normOffset + 1], normalData[normOffset + 2]);

            size_t texOffset = i * 2;
            glm::vec2 texCoord(texCoordData[texOffset + 0], texCoordData[texOffset + 1]);


            mesh.vertices.push_back({pos, norm, texCoord});
        }

        const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];
        tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];

        unsigned short* indices = reinterpret_cast<unsigned short*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
        std::cout << "target: " << indexAccessor.componentType << std::endl;
        for (size_t i = 0; i < indexAccessor.count; ++i) {
            mesh.indices.push_back((int) indices[i]);
        }
    }
    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.vertexBuffer->setLayout(layout);
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    return mesh;
}


void processNode(tinygltf::Model &gltfModel, tinygltf::Node &node, std::shared_ptr<Model> &model)
{
    if ((node.mesh >= 0) && (node.mesh < gltfModel.meshes.size()))
    {
        std::cout << "Adding mesh" << std::endl;
        model->meshes.push_back(processMesh(gltfModel, gltfModel.meshes[node.mesh]));
    }

    for (size_t i = 0; i < node.children.size(); ++i)
    {
        processNode(gltfModel, gltfModel.nodes[node.children[i]], model);
    }
}


Model::Pointer loadModel(std::string const &file)
{

    Model::Pointer geometry = std::make_shared<Model>();


    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file);

    if (!warn.empty())
    {
        std::cout << "Warn: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "Err: " << err << std::endl;
    }


    if (!ret)
    {
        std::cout << "Failed to parse GlTF" << std::endl;
        return geometry;
    }


    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        processNode(model, model.nodes[scene.nodes[i]], geometry);
    }

    return geometry;
}
