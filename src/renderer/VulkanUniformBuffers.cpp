#include "VulkanUniformBuffers.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <cstring>

static uint32_t FindMemoryType(VkPhysicalDevice phys, uint32_t typeFilter, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(phys, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
    {
        if ((typeFilter & (1u << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props)
            return i;
    }
    return UINT32_MAX;
}

VulkanUniformBuffers::VulkanUniformBuffers(VulkanDevice* device, uint32_t framesInFlight, VkDeviceSize bufferSize)
    : m_Device(device), m_BufferSize(bufferSize)
{
    m_Buffers.resize(framesInFlight);
    m_Memories.resize(framesInFlight);
    m_Mapped.resize(framesInFlight);

    VkDevice vkDevice = m_Device->GetHandle();
    VkPhysicalDevice phys = m_Device->GetPhysicalDevice();

    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        VkBufferCreateInfo bufInfo{};
        bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufInfo.size = m_BufferSize;
        bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(vkDevice, &bufInfo, nullptr, &m_Buffers[i]) != VK_SUCCESS)
            LOG_ERROR("Failed to create uniform buffer!");

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(vkDevice, m_Buffers[i], &memReq);

        uint32_t memType = FindMemoryType(
            phys,
            memReq.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        if (memType == UINT32_MAX)
            LOG_ERROR("Failed to find memory type for uniform buffer!");

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = memType;

        if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &m_Memories[i]) != VK_SUCCESS)
            LOG_ERROR("Failed to allocate uniform buffer memory!");

        vkBindBufferMemory(vkDevice, m_Buffers[i], m_Memories[i], 0);

        // Map once (persistent mapping)
        if (vkMapMemory(vkDevice, m_Memories[i], 0, m_BufferSize, 0, &m_Mapped[i]) != VK_SUCCESS)
            LOG_ERROR("Failed to map uniform buffer memory!");
    }

    LOG_INFO("Per-frame uniform buffers created + mapped.");
}

VulkanUniformBuffers::~VulkanUniformBuffers()
{
    VkDevice vkDevice = m_Device->GetHandle();

    for (size_t i = 0; i < m_Buffers.size(); i++)
    {
        if (m_Mapped[i]) vkUnmapMemory(vkDevice, m_Memories[i]);
        if (m_Buffers[i]) vkDestroyBuffer(vkDevice, m_Buffers[i], nullptr);
        if (m_Memories[i]) vkFreeMemory(vkDevice, m_Memories[i], nullptr);
    }
}

void VulkanUniformBuffers::Update(uint32_t frameIndex, const void* data, VkDeviceSize size)
{
    std::memcpy(m_Mapped[frameIndex], data, static_cast<size_t>(size));
}
