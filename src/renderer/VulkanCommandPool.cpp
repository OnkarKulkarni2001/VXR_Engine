#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

VulkanCommandPool::VulkanCommandPool(VulkanDevice* device)
    : m_Device(device)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_Device->GetGraphicsQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(
        m_Device->GetHandle(),
        &poolInfo,
        nullptr,
        &m_CommandPool) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create command pool!");
    }
    else
    {
        LOG_INFO("Command pool created.");
    }
}

VulkanCommandPool::~VulkanCommandPool()
{
    if (m_CommandPool)
        vkDestroyCommandPool(m_Device->GetHandle(), m_CommandPool, nullptr);
}
