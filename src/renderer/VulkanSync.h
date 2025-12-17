#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;

class VulkanSync
{
public:
    VulkanSync(VulkanDevice* device,
        uint32_t maxFramesInFlight,
        uint32_t swapchainImageCount);
    ~VulkanSync();

    // Per-frame
    VkSemaphore& GetImageAvailableSemaphore();
    VkFence& GetInFlightFence();

    uint32_t GetCurrentFrame() const { return m_CurrentFrame; }

    // Per-swapchain-image
    VkSemaphore& GetRenderFinishedSemaphore(uint32_t imageIndex);

    void AdvanceFrame();


private:
    VulkanDevice* m_Device = nullptr;

    uint32_t m_MaxFramesInFlight = 2;
    uint32_t m_CurrentFrame = 0;

    // Per-frame
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkFence>     m_InFlightFences;

    // Per-image
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
};
