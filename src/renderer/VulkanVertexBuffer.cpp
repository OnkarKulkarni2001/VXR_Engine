#include "VulkanVertexBuffer.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <cstring>     // memcpy
#include <stdexcept>

VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, const void* data, VkDeviceSize size)
    : m_Device(device), m_Size(size)
{
    if (!m_Device || !data || size == 0)
        throw std::runtime_error("VulkanVertexBuffer: invalid ctor args");

    // ------------------------------------------------------------
    // A) Staging buffer (CPU visible)
    // ------------------------------------------------------------
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

    m_Device->CreateBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingMemory
    );

    // Map + copy vertex data into staging
    void* mapped = nullptr;
    vkMapMemory(m_Device->GetHandle(), stagingMemory, 0, size, 0, &mapped);
    std::memcpy(mapped, data, static_cast<size_t>(size));
    vkUnmapMemory(m_Device->GetHandle(), stagingMemory);

    // ------------------------------------------------------------
    // B) Final vertex buffer (GPU local)
    // ------------------------------------------------------------
    m_Device->CreateBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_Buffer,
        m_Memory
    );

    // ------------------------------------------------------------
    // C) Copy staging -> GPU local
    // ------------------------------------------------------------
    m_Device->CopyBuffer(stagingBuffer, m_Buffer, size);

    // Staging is no longer needed after the copy
    vkDestroyBuffer(m_Device->GetHandle(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->GetHandle(), stagingMemory, nullptr);

    LOG_INFO("Vertex buffer created (DEVICE_LOCAL via staging).");
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    if (!m_Device) return;

    if (m_Buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_Device->GetHandle(), m_Buffer, nullptr);

    if (m_Memory != VK_NULL_HANDLE)
        vkFreeMemory(m_Device->GetHandle(), m_Memory, nullptr);
}
