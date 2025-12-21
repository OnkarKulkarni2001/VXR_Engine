#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;

class VulkanPipeline
{
public:
    VulkanPipeline(VulkanDevice* device,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderPass,
        const std::vector<VkDescriptorSetLayout>& setLayouts,
        const std::string& vertSpvPath,
        const std::string& fragSpvPath);


    ~VulkanPipeline();

    VkPipeline       GetHandle() const { return m_Pipeline; }
    VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

private:
    VulkanDevice* m_Device = nullptr;

    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline       m_Pipeline = VK_NULL_HANDLE;

private:
    static VkShaderModule CreateShaderModule(VkDevice device, const std::string& spvPath);
};
