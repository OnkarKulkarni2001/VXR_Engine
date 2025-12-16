#pragma once
#include "Window.h"

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanDepthBuffer;
class VulkanMSAAColorBuffer;
class VulkanRenderPass;
class VulkanFramebuffers;
class VulkanCommandPool;
class VulkanCommandBuffers;
class VulkanSync;

class Application {
public:
    Application();
    ~Application();

    void Run();

    void DrawFrame();

private:
    Window* m_Window = nullptr;

    VulkanInstance* m_Instance = nullptr;
    VulkanDevice* m_Device = nullptr;
    VulkanSwapchain* m_Swapchain = nullptr;
    VulkanDepthBuffer* m_DepthBuffer = nullptr;
    VulkanMSAAColorBuffer* m_MSAAColor = nullptr;
    VulkanRenderPass* m_RenderPass = nullptr;
    VulkanFramebuffers* m_Framebuffers = nullptr;
    VulkanCommandPool* m_CommandPool = nullptr;
    VulkanCommandBuffers* m_CommandBuffers = nullptr;
    VulkanSync* m_Sync = nullptr;
    VkSurfaceKHR          m_Surface = VK_NULL_HANDLE;
};
