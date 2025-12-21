#pragma once
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

class VulkanDevice;
class Mesh;


class ModelLoader
{
public:
    // Loads a model file and returns GPU-ready meshes
    static std::vector<std::unique_ptr<Mesh>> LoadStaticModel(
        VulkanDevice* device,
        const std::string& path
    );
};
