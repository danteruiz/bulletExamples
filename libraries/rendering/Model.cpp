#include "Model.h"

#include "Buffer.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

static std::string shaderPath("C:/Users/dante/code/rendering-examples/resources/shaders/");
static const std::string vertexShader = shaderPath + "simple.vs";
static const std::string fragmentShader = shaderPath + "simple.fs";

Mesh processMesh(aiMesh *aMesh, aiScene const *scene)
{
    Mesh mesh;

    for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
    {
        auto pos = aMesh->mVertices[i];
        auto norm = aMesh->mNormals[i];

        Vertex vertex({ pos.x, pos.y, pos.z }, { norm.x, norm.y, norm.z });
        mesh.vertices.push_back(vertex);
    }


    for (unsigned int i = 0; i < aMesh->mNumFaces; ++i)
    {
        aiFace face = aMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    mesh.vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.size(), mesh.vertices.data());
    mesh.indexBuffer = std::make_shared<Buffer>(Buffer::ELEMENT, mesh.indices.size() * sizeof(int), mesh.indices.size(), mesh.indices.data());

    return mesh;
}


void processNode(aiNode *node, aiScene const *scene, std::shared_ptr<Geometry> &model)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(processMesh(mesh, scene));
        std::cout << "Adding mesh" << std::endl;
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, model);
    }
}


std::shared_ptr<Geometry> loadModel(std::string const &file)
{
    Assimp::Importer importer;
    aiScene const *scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    std::shared_ptr<Geometry> geometry = std::make_shared<Geometry>();
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Failed loading model" << std::endl;
        return geometry;
    }


    processNode(scene->mRootNode, scene, geometry);

    std::cout << "Finished Load Model: " << file << std::endl;

    geometry->shader = std::make_shared<Shader>(fragmentShader, vertexShader);
    return geometry;
}
