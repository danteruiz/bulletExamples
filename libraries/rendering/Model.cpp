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

#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <vector>
#include <string>


std::string def = "#define HAS_";
static std::string resources = RESOURCE_PATH;
static const std::string shaderPath = std::string(RESOURCE_PATH) + "shaders/";
static const std::string VERTEX_SHADER = shaderPath + "pbr.vs";
static const std::string FRAGMENT_SHADER = shaderPath + "pbr.fs";

std::shared_ptr<Texture> loadMaterialTexture(tinygltf::Model &model, int index, std::string materialName, std::string& defines)
{
    if (index < 0)
    {
        return nullptr;
    }

    tinygltf::Texture const &gltfTexture = model.textures[index];
    tinygltf::Image &image = model.images[gltfTexture.source];

    defines += def + materialName + ";\n";
    return createTextureFromGLTF(image.width, image.height, image.component, image.bits, &image.image.at(0));
}

template <typename T>
void processIndexData(T const *gltfIndices, std::vector<uint32_t>& indices, size_t count, size_t startVertexIndex)
{
    for (size_t index = 0; index < count; index++)
    {
        indices.push_back((uint32_t) gltfIndices[index] + (uint32_t) startVertexIndex);
    }
}

Mesh processMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, tinygltf::Model &model, tinygltf::Mesh& gltfMesh)
{
    Mesh mesh;
    for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
    {

        Primitive prim;
        prim.indexStart = static_cast<uint32_t>(indices.size());
        prim.vertexStart = static_cast<uint32_t>(vertices.size());
        //std::cout << "vertex start: " << prim.vertexStart << std::endl;
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
            vertices.push_back({pos, norm, texCoord});
        }

        prim.vertexCount = static_cast<uint32_t>(positionAccess.count);


        if (primitive.indices >= 0)
        {
            const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];
            tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];

            void* const  indexData = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
            switch (indexAccessor.componentType)
            {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    processIndexData<uint8_t>(reinterpret_cast<uint8_t*>(indexData), indices, indexAccessor.count, prim.vertexStart);
                    break;

                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    processIndexData<uint16_t>(reinterpret_cast<uint16_t*>(indexData), indices, indexAccessor.count, prim.vertexStart);
                    break;

                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                case TINYGLTF_COMPONENT_TYPE_FLOAT:
                case TINYGLTF_COMPONENT_TYPE_INT:
                    processIndexData<uint32_t>(reinterpret_cast<uint32_t*>(indexData), indices, indexAccessor.count, prim.vertexStart);
                    break;

                default:
                    std::cout << "UNSUPPORTED TYPE: " << indexAccessor.componentType << std::endl;
            }
            prim.indexCount = static_cast<uint32_t>(indexAccessor.count);
        }
        prim.materialName = model.materials[primitive.material].name;
        mesh.primitives.push_back(prim);
    }

    return mesh;
}



glm::mat4 calculateLocalMatrix(glm::mat4& matrix, glm::vec3& translation, glm::vec3& scale, glm::quat& rotation)
{
    return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);// * matrix;
}

void processNode(tinygltf::Model &gltfModel, tinygltf::Node &node, std::shared_ptr<Model> &model, glm::mat4 parentMatrix = glm::mat4(1.0f))
{
    glm::mat4 finalMatrix;

    glm::mat4 matrix(1.0f);
    if (node.matrix.size() == 16)
    {
        matrix = glm::make_mat4x4(node.matrix.data());
    }

    glm::vec3 translation(0.0f);
    if (node.translation.size() == 3)
    {
        translation = glm::make_vec3(node.translation.data());
    }

    glm::quat rotation;
    if (node.rotation.size() == 4)
    {
        rotation = glm::make_quat(node.rotation.data());
    }

    glm::vec3 scale(1.0f);
    if (node.scale.size() == 3)
    {
        scale = glm::make_vec3(node.scale.data());
    }


    finalMatrix = parentMatrix * calculateLocalMatrix(matrix, translation, scale, rotation);

    if ((node.mesh >= 0) && (node.mesh < (int) gltfModel.meshes.size()))
    {
        Mesh mesh = processMesh(model->vertices, model->indices, gltfModel, gltfModel.meshes[node.mesh]);
        mesh.matrix = finalMatrix;
        model->meshes.push_back(mesh);
    }

    for (size_t i = 0; i < node.children.size(); ++i)
    {
        processNode(gltfModel, gltfModel.nodes[node.children[i]], model, finalMatrix);
    }
}

void getShadersAndMaterials(std::shared_ptr<Model>& model, tinygltf::Model gltfModel)
{
    for (auto& gltfMaterial : gltfModel.materials)
    {
        for (auto ext: gltfMaterial.extensions) {
            std::cout << "externals" << ext.first << std::endl;
        }

        std::string defines;
        std::shared_ptr<Material> material = std::make_shared<Material>();
        auto pbrMaterial = gltfMaterial.pbrMetallicRoughness;
        auto pbrBaseColor = pbrMaterial.baseColorFactor;
        material->albedo = glm::vec3(pbrBaseColor[0], pbrBaseColor[1], pbrBaseColor[2]);
        material->ao = (float) pbrBaseColor[3];
        auto emissiveFactor = gltfMaterial.emissiveFactor;
        material->emissive = glm::vec3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]);
        material->roughness = (float) pbrMaterial.roughnessFactor;
        material->metallic = (float) pbrMaterial.metallicFactor;
        material->albedoTexture = loadMaterialTexture(gltfModel, pbrMaterial.baseColorTexture.index, "ALBEDO_MAP", defines);
        material->normalTexture = loadMaterialTexture(gltfModel, gltfMaterial.normalTexture.index, "NORMAL_MAP", defines);
        material->emissiveTexture = loadMaterialTexture(gltfModel, gltfMaterial.emissiveTexture.index, "EMISSIVE_MAP", defines);
        material->occlusionTexture = loadMaterialTexture(gltfModel, gltfMaterial.occlusionTexture.index, "OCCLUSION_MAP", defines);
        material->metallicTexture = loadMaterialTexture(gltfModel, pbrMaterial.metallicRoughnessTexture.index, "METALLIC_ROUGHNESS_MAP", defines);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(FRAGMENT_SHADER, VERTEX_SHADER, defines);
        auto tuple = std::make_tuple(material, shader);
        model->materials[gltfMaterial.name] = tuple;
    }
}

Model::Pointer loadModel(std::string const &file)
{

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
        return nullptr;
    }


    Model::Pointer geometry = std::make_shared<Model>();
    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        processNode(model, model.nodes[scene.nodes[i]], geometry);
    }

    getShadersAndMaterials(geometry, model);

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex), (unsigned int) offsetof(Vertex, texCoord));
    auto& vertices = geometry->vertices;
    auto& indices = geometry->indices;

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, vertices.size() * sizeof(Vertex), vertices.size(), vertices.data());
    geometry->hasIndexBuffer = (indices.size() > 0);
    if (geometry->hasIndexBuffer)
    {
        geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, indices.size() * sizeof(uint32_t), indices.size(), indices.data());
    }
    geometry->vertexBuffer->setLayout(layout);

    return geometry;
}
