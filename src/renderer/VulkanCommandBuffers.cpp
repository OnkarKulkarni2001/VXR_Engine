#include "VulkanCommandBuffers.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffers.h"
#include "../core/Logger.h"

VulkanCommandBuffers::VulkanCommandBuffers(
    VulkanDevice* device,
    VulkanCommandPool* commandPool,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderPass,
    VulkanFramebuffers* framebuffers)
    : m_Device(device)
{
    m_CommandBuffers.resize(swapchain->GetImageViews().size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->GetHandle();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if (vkAllocateCommandBuffers(
        m_Device->GetHandle(),
        &allocInfo,
        m_CommandBuffers.data()) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to allocate command buffers!");
        return;
    }

    for (uint32_t i = 0; i < m_CommandBuffers.size(); i++)
    {
        RecordCommandBuffer(
            m_CommandBuffers[i],
            i,
            swapchain,
            renderPass,
            framebuffers
        );
    }

    LOG_INFO("Command buffers recorded.");
}

void VulkanCommandBuffers::RecordCommandBuffer(
    VkCommandBuffer cmd,
    uint32_t imageIndex,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderPass,
    VulkanFramebuffers* framebuffers)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearValues[2];
    clearValues[0].color = { {0.1f, 0.1f, 0.1f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = renderPass->GetHandle();
    rpBegin.framebuffer = framebuffers->GetFramebuffers()[imageIndex];
    rpBegin.renderArea.offset = { 0, 0 };
    rpBegin.renderArea.extent = swapchain->GetExtent();
    rpBegin.clearValueCount = 2;
    rpBegin.pClearValues = clearValues;

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to record command buffer!");
    }
}
