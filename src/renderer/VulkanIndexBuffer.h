#pragma once
#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanIndexBuffer
{
public:
    VulkanIndexBuffer(VulkanDevice* device,
        const void* data,
        VkDeviceSize size);

    ~VulkanIndexBuffer();

    VkBuffer GetBuffer() const { return m_Buffer; }
    uint32_t GetIndexCount() const { return m_IndexCount; }

private:
    VulkanDevice* m_Device = nullptr;

    VkBuffer       m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

    uint32_t m_IndexCount = 0;
};
