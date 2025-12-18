#pragma once
#include <vulkan/vulkan.h>
#include <cstddef>

class VulkanDevice;

class VulkanVertexBuffer
{
public:
    // data: pointer to your vertex array (Vertex*)
    // size: bytes (vertexCount * sizeof(Vertex))
    VulkanVertexBuffer(VulkanDevice* device, const void* data, VkDeviceSize size);
    ~VulkanVertexBuffer();

    VkBuffer GetBuffer() const { return m_Buffer; }
    VkDeviceSize GetSize() const { return m_Size; }

private:
    VulkanDevice* m_Device = nullptr;
    VkBuffer       m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkDeviceSize   m_Size = 0;
};
