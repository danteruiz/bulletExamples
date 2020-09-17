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

std::shared_ptr<Texture> loadMaterialTexture(tinygltf::Model  &model, int index)
{
    if (index < 0)
    {
        return nullptr;
    }

    tinygltf::Texture const &gltfTexture = model.textures[index];
    std::cout << "Creating Texuture: " << gltfTexture.name << std::endl;
    tinygltf::Image &image = model.images[gltfTexture.source];

    return createTextureFromGLTF(image.width, image.height, image.component, image.bits, &image.image.at(0));
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

        const tinygltf::Accessor &positionAccess = model.accessors[positionIndex];
        const tinygltf::Accessor &normalAccess = model.accessors[normalIndex];
        const tinygltf::Accessor &texCoordAccess = model.accessors[texCoordIndex];

        tinygltf::BufferView const &positionBufferView = model.bufferViews[positionAccess.bufferView];
        tinygltf::BufferView const &normalBufferView = model.bufferViews[normalAccess.bufferView];
        tinygltf::BufferView const &texCoordBufferView = model.bufferViews[texCoordAccess.bufferView];

        tinygltf::Buffer &positionBuffer = model.buffers[positionBufferView.buffer];
        tinygltf::Buffer &normalBuffer = model.buffers[normalBufferView.buffer];
        tinygltf::Buffer &texCoordBuffer = model.buffers[texCoordBufferView.buffer];

        float* const positionData = reinterpret_cast<float*> (&positionBuffer.data[positionBufferView.byteOffset + positionAccess.byteOffset]);
        float* const normalData = reinterpret_cast<float*> (&normalBuffer.data[normalBufferView.byteOffset + normalAccess.byteOffset]);
        float* const texCoordData = reinterpret_cast<float*> (&texCoordBuffer.data[texCoordBufferView.byteOffset + texCoordAccess.byteOffset]);


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
        for (size_t i = 0; i < indexAccessor.count; ++i) {
            mesh.indices.push_back((int) indices[i]);
        }


        // materials

        std::shared_ptr<Material> material = std::make_shared<Material>();
        tinygltf::Material const &gltfMaterial = model.materials[primitive.material];

        std::cout << "Material name: " << gltfMaterial.name << std::endl;

        auto pbrMaterial = gltfMaterial.pbrMetallicRoughness;
        auto pbrBaseColor = pbrMaterial.baseColorFactor;
        material->albedo = glm::vec3(pbrBaseColor[0], pbrBaseColor[1], pbrBaseColor[2]);
        material->ao = (float) pbrBaseColor[3];
        auto emissiveFactor = gltfMaterial.emissiveFactor;
        material->emissive = glm::vec3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);
        material->roughness = (float) pbrMaterial.roughnessFactor;
        material->metallic = (float) pbrMaterial.metallicFactor;
        material->albedoTexture = loadMaterialTexture(model, pbrMaterial.baseColorTexture.index);
        material->normalTexture = loadMaterialTexture(model, gltfMaterial.normalTexture.index);
        material->emissiveTexture = loadMaterialTexture(model, gltfMaterial.emissiveTexture.index);
        material->occlusionTexture = loadMaterialTexture(model, gltfMaterial.occlusionTexture.index);
        material->metallicTexture = loadMaterialTexture(model, pbrMaterial.metallicRoughnessTexture.index);

        std::cout << "material roughness: " << material->roughness << std::endl;
        std::cout << "material metallic: " << material->metallic << std::endl;

        mesh.material = material;
    }
    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex), (unsigned int) offsetof(Vertex, texCoord));
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
