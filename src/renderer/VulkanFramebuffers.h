#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanSwapchain;
class VulkanDepthBuffer;
class VulkanMSAAColorBuffer;
class VulkanRenderPass;

class VulkanFramebuffers
{
public:
    VulkanFramebuffers(VulkanDevice* device,
        VulkanSwapchain* swapchain,
        VulkanRenderPass* renderPass,
        VulkanDepthBuffer* depthBuffer,
        VulkanMSAAColorBuffer* msaaColor);
    ~VulkanFramebuffers();

    const std::vector<VkFramebuffer>& GetFramebuffers() const { return m_Framebuffers; }

private:
    VulkanDevice* m_Device = nullptr;
    VulkanSwapchain* m_Swapchain = nullptr;
    VulkanRenderPass* m_RenderPass = nullptr;
    VulkanDepthBuffer* m_DepthBuffer = nullptr;
    VulkanMSAAColorBuffer* m_MSAAColor = nullptr;

    std::vector<VkFramebuffer> m_Framebuffers;
};
