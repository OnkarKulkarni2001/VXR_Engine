#pragma once
#include <vulkan/vulkan.h>

class VulkanDevice;

enum class CommandPoolType
{
    Graphics,
    Transfer
};

class VulkanCommandPool
{
public:
    VulkanCommandPool(VulkanDevice* device, CommandPoolType type);
    ~VulkanCommandPool();

    VkCommandPool GetHandle() const { return m_CommandPool; }

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer cmd);

private:
    VulkanDevice* m_Device = nullptr;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    CommandPoolType m_Type;
};
