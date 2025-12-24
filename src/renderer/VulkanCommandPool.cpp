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

VkCommandBuffer VulkanCommandPool::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandPool = m_CommandPool;
    alloc.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(m_Device->GetHandle(), &alloc, &cmd);

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin);
    return cmd;
}

void VulkanCommandPool::EndSingleTimeCommands(VkCommandBuffer cmd)
{
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_Device->GetGraphicsQueue());

    vkFreeCommandBuffers(
        m_Device->GetHandle(),
        m_CommandPool,
        1,
        &cmd
    );
}