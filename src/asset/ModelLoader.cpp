#include "ModelLoader.h"

#include "renderer/Mesh.h"
#include "renderer/Vertex3D.h"
#include "renderer/VulkanDevice.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <stdexcept>
#include <vector>
#include <cstdint>

static std::string FindTextureRecursive(
    const std::filesystem::path& root,
    const std::filesystem::path& filename)
{
    for (auto& p : std::filesystem::recursive_directory_iterator(root))
    {
        if (!p.is_regular_file()) continue;

        if (p.path().filename() == filename)
            return p.path().string();
    }
    return "";
}


static std::string GetTexturePath(
    const aiMaterial* mat,
    aiTextureType type,
    const std::string& baseDir)
{
    if (!mat) return "";

    aiString str;
    if (mat->GetTexture(type, 0, &str) != AI_SUCCESS)
        return "";

    std::filesystem::path texPath(str.C_Str());
    std::filesystem::path modelDir(baseDir);

    // Strip artist absolute paths
    if (texPath.is_absolute())
        texPath = texPath.filename();

    // 1️ Try direct resolution
    std::filesystem::path direct = modelDir / texPath;
    if (std::filesystem::exists(direct))
        return direct.lexically_normal().string();

    // 2️ Fallback: search recursively (maps/, textures/, etc.)
    std::string found = FindTextureRecursive(modelDir, texPath.filename());
    if (!found.empty())
        return found;

    // 3️ Give up gracefully
    return "";
}



static void ProcessMesh(
    aiMesh* mesh,
    const aiScene* scene,
    VulkanDevice* device,
    const std::string& baseDir,
    std::vector<LoadedMesh>& out
)
{
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex3D v{};
        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->HasNormals())
            v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->mTextureCoords[0])
            v.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

        vertices.push_back(v);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    LoadedMesh lm{};
    lm.mesh = std::make_unique<Mesh>(
        device,
        vertices.data(),
        sizeof(Vertex3D) * vertices.size(),
        sizeof(Vertex3D),
        indices.data(),
        sizeof(uint32_t) * indices.size(),
        (uint32_t)indices.size()
    );

    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

    lm.albedoPath = GetTexturePath(mat, aiTextureType_DIFFUSE, baseDir);
    lm.normalPath = GetTexturePath(mat, aiTextureType_NORMALS, baseDir);

    out.push_back(std::move(lm));
}


std::vector<LoadedMesh> ModelLoader::LoadStaticModel(
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
        throw std::runtime_error("Failed to load model: " + path);

    std::string baseDir = path.substr(0, path.find_last_of("/\\"));

    std::vector<LoadedMesh> meshes;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        ProcessMesh(scene->mMeshes[i], scene, device, baseDir, meshes);
    }

    return meshes;
}