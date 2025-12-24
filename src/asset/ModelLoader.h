#pragma once
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

class VulkanDevice;
class Mesh;
class MaterialTemplate;
class VulkanMaterialDescriptors;
class VulkanTexture2D;

struct LoadedMesh
{
    std::unique_ptr<Mesh> mesh;
    std::string albedoPath;
    std::string normalPath;
};

class ModelLoader
{
public:
    static std::vector<LoadedMesh> LoadStaticModel(
        VulkanDevice* device,
        const std::string& path
    );
};
