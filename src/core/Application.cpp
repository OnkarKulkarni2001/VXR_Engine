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


Application::Application()
{
    m_Window = new Window(1280, 720, "VXR Engine");
    Logger::Log("Application Initialized");
}

Application::~Application()
{
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

    // 2) Create Vulkan device (needs instance + window surface)
    VkSurfaceKHR surface = m_Window->CreateSurface(m_Instance->GetHandle());

    m_Device = new VulkanDevice(
        m_Instance->GetHandle(),
        surface
    );


    // 3) Create swapchain
    m_Swapchain = new VulkanSwapchain(
        m_Instance->GetHandle(),
        m_Device,
        m_Window->GetSurface(),
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

    // 8) Command Pool & Command Buffers Creation & Recording
    m_CommandPool = new VulkanCommandPool(m_Device);
    m_CommandBuffers = new VulkanCommandBuffers(
        m_Device, m_CommandPool, m_Swapchain, m_RenderPass, m_Framebuffers
    );

    // --- MAIN LOOP ---
    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();

        // Later: draw frame here
    }

    vkDeviceWaitIdle(m_Device->GetHandle());
}
