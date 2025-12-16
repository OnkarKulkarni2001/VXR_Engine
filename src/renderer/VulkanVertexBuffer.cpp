#include "VulkanVertexBuffer.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

VulkanVertexBuffer::VulkanVertexBuffer(
    VulkanDevice* device,
    const void* vertexData,
    VkDeviceSize size
)
    : m_Device(device)
{
    VkDevice vkDevice = m_Device->GetHandle();

    // 1) Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create vertex buffer!");
        return;
    }

    // 2) Allocate memory
    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(vkDevice, m_Buffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex =
        m_Device->FindMemoryType(
            memReq.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to allocate vertex buffer memory!");
        return;
    }

    vkBindBufferMemory(vkDevice, m_Buffer, m_Memory, 0);

    // 3) Upload data
    void* data;
    vkMapMemory(vkDevice, m_Memory, 0, size, 0, &data);
    memcpy(data, vertexData, (size_t)size);
    vkUnmapMemory(vkDevice, m_Memory);

    LOG_INFO("Vertex buffer created.");
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    VkDevice vkDevice = m_Device->GetHandle();

    if (m_Buffer)
        vkDestroyBuffer(vkDevice, m_Buffer, nullptr);

    if (m_Memory)
        vkFreeMemory(vkDevice, m_Memory, nullptr);
}
