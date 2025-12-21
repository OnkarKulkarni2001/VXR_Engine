#pragma once
#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanMaterialDescriptors;
class MaterialTemplate;

class MaterialInstance
{
public:
    // You pass VkImageView + VkSampler for albedo and normal
    MaterialInstance(
        VulkanDevice* device,
        VulkanMaterialDescriptors* materialPool,
        MaterialTemplate* templ,
        VkImageView albedoView, VkSampler albedoSampler,
        VkImageView normalView, VkSampler normalSampler);

    ~MaterialInstance() = default;

    VkDescriptorSet GetDescriptorSet() const { return m_Set1; }
    // convenience: RenderQueue wants pipeline
    class VulkanPipeline* GetPipeline() const;

private:
    VulkanDevice* m_Device = nullptr;
    MaterialTemplate* m_Template = nullptr;
    VkDescriptorSet m_Set1 = VK_NULL_HANDLE;
};
