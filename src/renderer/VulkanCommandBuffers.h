#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanCommandPool;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanFramebuffers;

class VulkanCommandBuffers
{
public:
    VulkanCommandBuffers(
        VulkanDevice* device,
        VulkanCommandPool* commandPool,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderPass,
        VulkanFramebuffers* framebuffers
    );

    const std::vector<VkCommandBuffer>& GetBuffers() const { return m_CommandBuffers; }

private:
    VulkanDevice* m_Device = nullptr;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    void RecordCommandBuffer(
        VkCommandBuffer cmd,
        uint32_t imageIndex,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderPass,
        VulkanFramebuffers* framebuffers
    );
};
