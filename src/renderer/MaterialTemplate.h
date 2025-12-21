#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanPipeline;

class MaterialTemplate
{
public:
    MaterialTemplate(
        VulkanDevice* device,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderPass,
        VkDescriptorSetLayout globalSetLayout, // set 0
        const std::string& vertSpv,
        const std::string& fragSpv);

    ~MaterialTemplate();

    VulkanPipeline* GetPipeline() const { return m_Pipeline; }
    VkDescriptorSetLayout GetMaterialSetLayout() const { return m_MaterialSetLayout; }

private:
    VulkanDevice* m_Device = nullptr;
    VulkanPipeline* m_Pipeline = nullptr;

    VkDescriptorSetLayout m_MaterialSetLayout = VK_NULL_HANDLE; // set 1
};
