#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

class Mesh;
class VulkanPipeline;
class MaterialInstance;
struct RenderObject;

struct RenderCommand
{
    Mesh* mesh = nullptr;
    VulkanPipeline* pipeline = nullptr;
    glm::mat4 model;
	MaterialInstance* material = nullptr;
    VkDescriptorSet materialSet = VK_NULL_HANDLE;
};

class RenderQueue
{
public:
    void Clear();
    void Submit(const RenderObject& obj);
    const std::vector<RenderCommand>& GetCommands() const;

private:
    std::vector<RenderCommand> m_Commands;
};
