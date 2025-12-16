#pragma once
#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(
        VulkanDevice* device,
        const void* vertexData,
        VkDeviceSize size
    );

    ~VulkanVertexBuffer();

    VkBuffer GetBuffer() const { return m_Buffer; }

private:
    VulkanDevice* m_Device = nullptr;

    VkBuffer       m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
};
