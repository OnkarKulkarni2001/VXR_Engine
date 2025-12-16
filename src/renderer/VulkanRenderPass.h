#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanRenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, VkFormat colorFormat, VkFormat depthFormat);
    ~VulkanRenderPass();

    VkRenderPass GetHandle() const { return m_RenderPass; }

private:
    VulkanDevice* m_Device = nullptr;
    VkRenderPass  m_RenderPass = VK_NULL_HANDLE;
};
