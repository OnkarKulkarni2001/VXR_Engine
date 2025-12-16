#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;

class VulkanDepthBuffer
{
public:
    VulkanDepthBuffer(VulkanDevice* device, VkExtent2D extent);
    ~VulkanDepthBuffer();

    VkImageView GetImageView() const { return m_ImageView; }
    VkFormat    GetFormat()    const { return m_Format; }

private:
    VulkanDevice* m_Device = nullptr;

    VkImage        m_Image = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkImageView    m_ImageView = VK_NULL_HANDLE;

    VkFormat       m_Format = VK_FORMAT_UNDEFINED;
    VkExtent2D     m_Extent{};
};
