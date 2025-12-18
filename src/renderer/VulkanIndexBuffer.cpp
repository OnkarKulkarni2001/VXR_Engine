#include "VulkanIndexBuffer.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <cstring>

VulkanIndexBuffer::VulkanIndexBuffer(
    VulkanDevice* device,
    const void* data,
    VkDeviceSize size)
    : m_Device(device)
{
    m_IndexCount = static_cast<uint32_t>(size / sizeof(uint32_t));

    // -------------------------
    // 1) Staging buffer (CPU visible)
    // -------------------------
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    m_Device->CreateBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingMemory
    );

    void* mapped = nullptr;
    vkMapMemory(m_Device->GetHandle(), stagingMemory, 0, size, 0, &mapped);
    memcpy(mapped, data, static_cast<size_t>(size));
    vkUnmapMemory(m_Device->GetHandle(), stagingMemory);

    // -------------------------
    // 2) GPU-only index buffer
    // -------------------------
    m_Device->CreateBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_Buffer,
        m_BufferMemory
    );

    // -------------------------
    // 3) Copy staging → index buffer
    // -------------------------
    m_Device->CopyBuffer(stagingBuffer, m_Buffer, size);

    // -------------------------
    // 4) Cleanup staging
    // -------------------------
    vkDestroyBuffer(m_Device->GetHandle(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->GetHandle(), stagingMemory, nullptr);

    LOG_INFO("Index buffer created (staging → device local).");
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    if (m_Buffer)
        vkDestroyBuffer(m_Device->GetHandle(), m_Buffer, nullptr);

    if (m_BufferMemory)
        vkFreeMemory(m_Device->GetHandle(), m_BufferMemory, nullptr);
}
