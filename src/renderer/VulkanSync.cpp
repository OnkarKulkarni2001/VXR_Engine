#include "VulkanSync.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

VulkanSync::VulkanSync(VulkanDevice* device,
    uint32_t maxFramesInFlight,
    uint32_t swapchainImageCount)
    : m_Device(device),
    m_MaxFramesInFlight(maxFramesInFlight)
{
    // Per-frame sync
    m_ImageAvailableSemaphores.resize(maxFramesInFlight);
    m_InFlightFences.resize(maxFramesInFlight);

    // Per-swapchain-image sync
    m_RenderFinishedSemaphores.resize(swapchainImageCount);

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Create per-frame semaphores + fences
    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        if (vkCreateSemaphore(m_Device->GetHandle(), &semInfo, nullptr,
            &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device->GetHandle(), &fenceInfo, nullptr,
                &m_InFlightFences[i]) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create per-frame sync objects!");
        }
    }

    // Create per-image render-finished semaphores
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        if (vkCreateSemaphore(m_Device->GetHandle(), &semInfo, nullptr,
            &m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create render-finished semaphore!");
        }
    }

    LOG_INFO("VulkanSync created (per-frame + per-image sync).");
}

VulkanSync::~VulkanSync()
{
    for (auto s : m_ImageAvailableSemaphores)
        vkDestroySemaphore(m_Device->GetHandle(), s, nullptr);

    for (auto s : m_RenderFinishedSemaphores)
        vkDestroySemaphore(m_Device->GetHandle(), s, nullptr);

    for (auto f : m_InFlightFences)
        vkDestroyFence(m_Device->GetHandle(), f, nullptr);
}

VkSemaphore& VulkanSync::GetImageAvailableSemaphore()
{
    return m_ImageAvailableSemaphores[m_CurrentFrame];
}

VkFence& VulkanSync::GetInFlightFence()
{
    return m_InFlightFences[m_CurrentFrame];
}

VkSemaphore& VulkanSync::GetRenderFinishedSemaphore(uint32_t imageIndex)
{
    return m_RenderFinishedSemaphores[imageIndex];
}

void VulkanSync::AdvanceFrame()
{
    m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
}
