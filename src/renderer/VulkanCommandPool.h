#pragma once
#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanCommandPool
{
public:
    VulkanCommandPool(VulkanDevice* device);
    ~VulkanCommandPool();

    VkCommandPool GetHandle() const { return m_CommandPool; }

private:
    VulkanDevice* m_Device = nullptr;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
};
