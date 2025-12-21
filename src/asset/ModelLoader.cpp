#include "ModelLoader.h"

#include "renderer/Mesh.h"
#include "renderer/Vertex3D.h"
#include "renderer/VulkanDevice.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <vector>
#include <cstdint>


static void ProcessMesh(
    aiMesh* mesh,
    VulkanDevice* device,
    std::vector<std::unique_ptr<Mesh>>& outMeshes
)
{
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex3D v{};

        // Position
        v.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // Normal (may not exist)
        if (mesh->HasNormals())
        {
            v.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        // UV (channel 0)
        if (mesh->mTextureCoords[0])
        {
            v.uv = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(v);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    outMeshes.push_back(
        std::make_unique<Mesh>(
            device,
            vertices.data(),
            sizeof(Vertex3D) * vertices.size(),
            sizeof(Vertex3D),
            indices.data(),
            sizeof(uint32_t) * indices.size(),
            static_cast<uint32_t>(indices.size())
        )
    );

}

std::vector<std::unique_ptr<Mesh>> ModelLoader::LoadStaticModel(
    VulkanDevice* device,
    const std::string& path
)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->mRootNode)
        throw std::runtime_error("Assimp failed to load model: " + path);

    std::vector<std::unique_ptr<Mesh>> meshes;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        ProcessMesh(scene->mMeshes[i], device, meshes);
    }

    return meshes;
}
