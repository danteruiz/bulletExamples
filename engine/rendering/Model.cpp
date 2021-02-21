#include "Model.h"

#include "Buffer.h"
#include "Shader.h"
#include "Material.h"
#include "Format.h"
#include "Texture.h"

#define TINYGLTF_IMPLEMENTATION
#include "external/tiny_gltf.h"

#include <spdlog/spdlog.h>
#include <StopWatch.h>

#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <vector>
#include <string>


std::string def = "#define HAS_";
static std::string resources = RESOURCE_PATH;
static const std::string shaderPath = std::string(RESOURCE_PATH) + "shaders/";
static const std::string VERTEX_SHADER = shaderPath + "pbr.vs";
static const std::string FRAGMENT_SHADER = shaderPath + "pbr.fs";

std::shared_ptr<Texture> loadMaterialTexture(tinygltf::Model &model, int index,
                                             std::string materialName,
                                             std::string& defines)
{
    if (index < 0)
    {
        return nullptr;
    }

    tinygltf::Texture const &gltfTexture = model.textures[index];
    tinygltf::Image &image = model.images[gltfTexture.source];

    defines += def + materialName + ";\n";
    return createTextureFromGLTF(image.width, image.height, image.component,
                                 image.bits, &image.image.at(0));
}

template <typename T>
void processIndexData(T const *gltfIndices, std::vector<uint32_t>& indices,
                      size_t count, size_t startVertexIndex)
{
    for (size_t index = 0; index < count; index++)
    {
        uint32_t indice = (uint32_t) gltfIndices[index]
            + (uint32_t) startVertexIndex;

        indices.push_back(indice);
    }
}

Mesh processMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
                 tinygltf::Model &model, tinygltf::Mesh& gltfMesh)
{
    //    spdlog::debug("Model::processMesh");
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

        const tinygltf::Accessor &positionAccess =
            model.accessors[positionIndex];

        const tinygltf::Accessor &normalAccess =
            model.accessors[normalIndex];

        const tinygltf::Accessor &texCoordAccess =
            model.accessors[texCoordIndex];

        tinygltf::BufferView const &positionBufferView =
            model.bufferViews[positionAccess.bufferView];

        tinygltf::BufferView const &normalBufferView =
            model.bufferViews[normalAccess.bufferView];

        tinygltf::BufferView const &texCoordBufferView =
            model.bufferViews[texCoordAccess.bufferView];

        tinygltf::Buffer &positionBuffer =
            model.buffers[positionBufferView.buffer];

        tinygltf::Buffer &normalBuffer =
            model.buffers[normalBufferView.buffer];

        tinygltf::Buffer &texCoordBuffer =
            model.buffers[texCoordBufferView.buffer];


        int positionBufferIndex = positionBufferView.byteOffset
            + positionAccess.byteOffset;

        float* const positionData =
            reinterpret_cast<float*> (&positionBuffer.data[positionBufferIndex]);

        float* const normalData = reinterpret_cast<float*>
            (&normalBuffer.data[normalBufferView.byteOffset
                                + normalAccess.byteOffset]);

        float* const texCoordData = reinterpret_cast<float*>
            (&texCoordBuffer.data[texCoordBufferView.byteOffset
                                  + texCoordAccess.byteOffset]);


        for (size_t i = 0; i < positionAccess.count; ++i)
        {
            size_t posOffset = i *  3;
            glm::vec3 pos(positionData[posOffset + 0],
                          positionData[posOffset + 1],
                          positionData[posOffset + 2]);

            size_t normOffset = i * 3;
            glm::vec3 norm(normalData[normOffset + 0],
                           normalData[normOffset + 1],
                           normalData[normOffset + 2]);

            size_t texOffset = i * 2;
            glm::vec2 texCoord(texCoordData[texOffset + 0],
                               texCoordData[texOffset + 1]);

            vertices.push_back({pos, norm, texCoord});
        }

        prim.vertexCount = static_cast<uint32_t>(positionAccess.count);


        if (primitive.indices >= 0)
        {
            const tinygltf::Accessor &indexAccessor =
                model.accessors[primitive.indices];
            const tinygltf::BufferView &indexBufferView =
                model.bufferViews[indexAccessor.bufferView];
            tinygltf::Buffer &indexBuffer =
                model.buffers[indexBufferView.buffer];

            void* const indexData =
                &indexBuffer.data[indexBufferView.byteOffset
                                  + indexAccessor.byteOffset];

            switch (indexAccessor.componentType)
            {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    processIndexData<uint8_t>(reinterpret_cast<uint8_t*>(indexData)
                                              ,indices, indexAccessor.count,
                                              prim.vertexStart);
                    break;

                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    processIndexData<uint16_t>(reinterpret_cast<uint16_t*>(indexData), indices,
                                               indexAccessor.count, prim.vertexStart);
                    break;

                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                case TINYGLTF_COMPONENT_TYPE_FLOAT:
                case TINYGLTF_COMPONENT_TYPE_INT:
                    processIndexData<uint32_t>(reinterpret_cast<uint32_t*>(indexData), indices,
                                               indexAccessor.count, prim.vertexStart);
                    break;

                default:
                    std::cout << "UNSUPPORTED TYPE: " << indexAccessor.componentType << std::endl;
            }
            prim.indexCount = static_cast<uint32_t>(indexAccessor.count);
        }
        prim.materialIndex = primitive.material;
        mesh.primitives.push_back(prim);
    }

    return mesh;
}



glm::mat4 calculateLocalMatrix(glm::mat4& matrix, glm::vec3& translation, glm::vec3& scale,
                               glm::quat& rotation)
{
    return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation)
        * glm::scale(glm::mat4(1.0f), scale);// * matrix;
}

void processNode(tinygltf::Model &gltfModel, tinygltf::Node &node, std::shared_ptr<Model> &model,
                 glm::mat4 parentMatrix = glm::mat4(1.0f))
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
        Mesh mesh = processMesh(model->vertices, model->indices, gltfModel,
                                gltfModel.meshes[node.mesh]);
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
    size_t materialCount = gltfModel.materials.size();
    for (size_t index = 0; index < materialCount; ++index)
    {
        const auto& gltfMaterial = gltfModel.materials[index];
        for (auto ext: gltfMaterial.extensions) {
            std::cout << "externals" << ext.first << std::endl;
            //  spdlog::debug("model: externals {}", ext.first);
        }

        //        spdlog::debug("material name: {}", gltfMaterial.name);
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
        material->albedoTexture = loadMaterialTexture(gltfModel, pbrMaterial.baseColorTexture.index,
                                                      "ALBEDO_MAP", defines);

        material->normalTexture = loadMaterialTexture(gltfModel, gltfMaterial.normalTexture.index,
                                                      "NORMAL_MAP", defines);

        material->emissiveTexture = loadMaterialTexture(gltfModel,
                                                        gltfMaterial.emissiveTexture.index,
                                                        "EMISSIVE_MAP", defines);

        material->occlusionTexture = loadMaterialTexture(gltfModel,
                                                         gltfMaterial.occlusionTexture.index,
                                                         "OCCLUSION_MAP", defines);

        material->metallicTexture = loadMaterialTexture(gltfModel,
                                                        pbrMaterial.metallicRoughnessTexture.index,
                                                        "METALLIC_ROUGHNESS_MAP", defines);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(FRAGMENT_SHADER, VERTEX_SHADER,
                                                                  defines);
        auto tuple = std::make_tuple(material, shader);
        model->materials[(uint32_t) index] = tuple;
    }
}

Model::Pointer loadGLTFModel(std::string const &file)
{

    ChronoStopWatch sw("loadModel");
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
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex),
                         (unsigned int) offsetof(Vertex, texCoord));
    auto& vertices = geometry->vertices;
    auto& indices = geometry->indices;

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, vertices.size() *
                                                      sizeof(Vertex), vertices.size(),
                                                      vertices.data());
    geometry->hasIndexBuffer = (indices.size() > 0);
    if (geometry->hasIndexBuffer)
    {
        geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, indices.size() *
                                                         sizeof(uint32_t), indices.size(),
                                                         indices.data());
    }
    geometry->vertexBuffer->setLayout(layout);

    return geometry;
}

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
    primitive.materialIndex = 0;

    mesh.primitives.push_back(primitive);

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, geometry->vertices.size()
                                                      * sizeof(Vertex), geometry->vertices.size(),
                                                      geometry->vertices.data());
    geometry->vertexBuffer->setLayout(layout);
    geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, geometry->indices.size()
                                                     * sizeof(int), geometry->indices.size(),
                                                     geometry->indices.data());

    //geometry->materials["default"] = std::make_shared<Material>();

    geometry->meshes.push_back(mesh);
    geometry->hasIndexBuffer = true;
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
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex),
                         (unsigned int) offsetof(Vertex,texCoord));

    geometry->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, geometry->vertices.size()
                                                      * sizeof(Vertex), geometry->vertices.size(),
                                                      geometry->vertices.data());
    geometry->vertexBuffer->setLayout(layout);
    geometry->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, geometry->indices.size()
                                                     * sizeof(int), geometry->indices.size(),
                                                     geometry->indices.data());
    geometry->meshes.push_back(mesh);

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
    primitive.materialIndex = 0;

    mesh.primitives.push_back(primitive);

     std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(Slots::POSITION, 3, sizeof(Vertex), 0);
    layout->setAttribute(Slots::NORMAL, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));
    layout->setAttribute(Slots::TEXCOORD, 2, sizeof(Vertex),
                         (unsigned int) offsetof(Vertex,texCoord));

    model->vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, model->vertices.size() *
                                                   sizeof(Vertex), model->vertices.size(),
                                                   model->vertices.data());
    model->vertexBuffer->setLayout(layout);
    model->indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, model->indices.size()
                                                  * sizeof(int), model->indices.size(),
                                                  model->indices.data());

    model->meshes.push_back(mesh);

    return model;
}

ModelCache::ModelCache()
{
    m_modelShapes[ModelShape::Cube] = buildCube();
    m_modelShapes[ModelShape::Sphere] = buildSphere();
    m_modelShapes[ModelShape::Quad] = buildQuad();
}


Model::Pointer ModelCache::loadModel(std::string const &file)
{
    auto modelIter = m_models.find(file);

    if (modelIter != m_models.end())
    {
        return modelIter->second;
    }

    Model::Pointer model = nullptr;

    model = loadGLTFModel(file);
    m_models[file] = model;
    return model;
}


Model::Pointer ModelCache::getModelShape(int modelCache)
{
    return m_modelShapes[modelCache];
}
