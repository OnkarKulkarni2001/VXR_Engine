#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;

class VulkanMSAAColorBuffer
{
public:
    VulkanMSAAColorBuffer(VulkanDevice* device, VkFormat colorFormat, VkExtent2D extent);
    ~VulkanMSAAColorBuffer();

    VkImageView GetImageView() const { return m_ImageView; }

private:
    VulkanDevice* m_Device = nullptr;

    VkImage        m_Image = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkImageView    m_ImageView = VK_NULL_HANDLE;

    VkFormat       m_Format = VK_FORMAT_UNDEFINED;
    VkExtent2D     m_Extent{};
};
