#include "Model.h"

#include "Buffer.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Mesh processMesh(aiMesh *aMesh, aiScene const *scene)
{
    Mesh mesh;

    for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
    {
        auto pos = aMesh->mVertices[i];
        auto norm = aMesh->mNormals[i];

        Vertex vertex({ pos.x, pos.y, pos.z }, { norm.x, norm.y, norm.z });

        auto textureCoords = aMesh->mTextureCoords[0];
        if (aMesh->mTextureCoords[0])
        {
            auto texCoords = textureCoords[i];
            vertex.uv = glm::vec2(texCoords.x, texCoords.y);
        }
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


Model::Pointer loadModel(std::string const &file)
{
    Assimp::Importer importer;
    aiScene const *scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    Modeel::Pointer geometry = std::make_shared<Model>();
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Failed loading model" << std::endl;
        return geometry;
    }


    processNode(scene->mRootNode, scene, geometry);

    std::cout << "Finished Load Model: " << file << std::endl;

    return geometry;
}
