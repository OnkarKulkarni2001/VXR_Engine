#include "Application.h"
#include "Logger.h"

#include "../renderer/VulkanInstance.h"
#include "../renderer/VulkanDevice.h"
#include "../renderer/VulkanSwapchain.h"
#include "../renderer/VulkanDepthBuffer.h"
#include "../renderer/VulkanMSAAColorBuffer.h"
#include "../renderer/VulkanRenderPass.h"
#include "../renderer/VulkanFramebuffers.h"
#include "../renderer/VulkanCommandPool.h"
#include "../renderer/VulkanCommandBuffers.h"
#include "../renderer/VulkanSync.h"

Application::Application()
{
    m_Window = new Window(1280, 720, "VXR Engine");
    Logger::Log("Application Initialized");
}

Application::~Application()
{
    // Destroy in reverse order of creation (important in Vulkan)
    delete m_Sync;
    delete m_CommandBuffers;
    delete m_CommandPool;

    delete m_Framebuffers;
    delete m_RenderPass;
    delete m_MSAAColor;
    delete m_DepthBuffer;
    delete m_Swapchain;

    delete m_Device;
    delete m_Instance;
    delete m_Window;
}

void Application::Run()
{
    // 1) Create Vulkan instance
    m_Instance = new VulkanInstance(true);

    // 2) Create surface + device
    VkSurfaceKHR surface = m_Window->CreateSurface(m_Instance->GetHandle());

    m_Device = new VulkanDevice(
        m_Instance->GetHandle(),
        surface
    );

    // 3) Create swapchain (use the same surface we just created)
    m_Swapchain = new VulkanSwapchain(
        m_Instance->GetHandle(),
        m_Device,
        surface,
        m_Window->GetWidth(),
        m_Window->GetHeight()
    );

    // EXTENT + FORMAT come from swapchain
    VkExtent2D extent = m_Swapchain->GetExtent();
    VkFormat   colorFormat = m_Swapchain->GetImageFormat();
    VkFormat   depthFormat = m_Device->FindDepthFormat();

    // 4) Depth buffer (MSAA)
    m_DepthBuffer = new VulkanDepthBuffer(
        m_Device,
        extent
    );

    // 5) MSAA color buffer
    m_MSAAColor = new VulkanMSAAColorBuffer(
        m_Device,
        colorFormat,
        extent
    );

    // 6) Render pass using color + depth + resolve
    m_RenderPass = new VulkanRenderPass(
        m_Device,
        colorFormat,
        depthFormat
    );

    // 7) Framebuffers (one per swapchain image)
    m_Framebuffers = new VulkanFramebuffers(
        m_Device,
        m_Swapchain,
        m_RenderPass,
        m_DepthBuffer,
        m_MSAAColor
    );

    // 8) Command Pool & Command Buffers
    m_CommandPool = new VulkanCommandPool(m_Device);

    m_CommandBuffers = new VulkanCommandBuffers(
        m_Device,
        m_CommandPool,
        m_Swapchain,
        m_RenderPass,
        m_Framebuffers
    );

    // 9) Sync
    // IMPORTANT: VulkanSync should internally create:
    // - imageAvailable semaphores per FRAME (maxFramesInFlight)
    // - renderFinished semaphores per SWAPCHAIN IMAGE
    // - fences per FRAME
    m_Sync = new VulkanSync(
        m_Device,
        2, // max frames in flight (standard engine value)
        static_cast<uint32_t>(m_Swapchain->GetImageViews().size())
    );

    // --- MAIN LOOP ---
    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(m_Device->GetHandle());
}

void Application::DrawFrame()
{
    // 1) Wait for this frame slot to be available (CPU-GPU pacing)
    VkFence& inFlightFence = m_Sync->GetInFlightFence();

    vkWaitForFences(
        m_Device->GetHandle(),
        1,
        &inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    vkResetFences(m_Device->GetHandle(), 1, &inFlightFence);

    // 2) Acquire a swapchain image
    uint32_t imageIndex = 0;

    VkSemaphore& imageAvailable = m_Sync->GetImageAvailableSemaphore(); // per-frame

    VkResult acquireResult = vkAcquireNextImageKHR(
        m_Device->GetHandle(),
        m_Swapchain->GetHandle(),
        UINT64_MAX,
        imageAvailable,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // Resize handling later; for now just skip the frame safely
        LOG_WARN("Swapchain out of date (resize). Skipping frame.");
        return;
    }
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR("Failed to acquire swapchain image!");
        return;
    }

    // 3) Submit command buffer
    VkSemaphore waitSemaphores[] = { imageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSemaphore& renderFinished = m_Sync->GetRenderFinishedSemaphore(imageIndex); // per-image
    VkSemaphore signalSemaphores[] = { renderFinished };

    VkCommandBuffer cmd = m_CommandBuffers->GetBuffers()[imageIndex];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(
        m_Device->GetGraphicsQueue(),
        1,
        &submitInfo,
        inFlightFence // fence handle (reference is fine)
    ) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to submit draw command buffer!");
        return;
    }

    // 4) Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { m_Swapchain->GetHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult presentResult = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
        // Resize handling later; for now just skip safely
        LOG_WARN("Swapchain out of date/suboptimal on present. Skipping.");
    }
    else if (presentResult != VK_SUCCESS)
    {
        LOG_ERROR("Failed to present swapchain image!");
    }

    // 5) Advance frame slot (only affects per-frame semaphores/fences)
    m_Sync->AdvanceFrame();
}
