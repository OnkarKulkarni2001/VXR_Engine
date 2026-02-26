#pragma once

#include "xr/OpenXRCommon.h"

#include <vector>
#include <string>

struct XRFrameInfo
{
    XrTime predictedDisplayTime = 0;
    XrDuration predictedDisplayPeriod = 0;
    bool shouldRender = false;
};

class OpenXRSession
{
public:
    OpenXRSession() = default;
    ~OpenXRSession();

    bool Create(
        XrInstance instance,
        XrSystemId systemId,
        VkInstance vkInstance,
        VkPhysicalDevice vkPhysicalDevice,
        VkDevice vkDevice,
        uint32_t graphicsQueueFamilyIndex,
        uint32_t graphicsQueueIndex = 0
    );

    void Destroy();

    // Call each frame to drive session state.
    void PollEvents();

    bool IsRunning() const { return m_Running; }
    bool ExitRequested() const { return m_ExitRequested; }

    XrSession GetSession() const { return m_Session; }
    XrSpace GetAppSpace() const { return m_AppSpace; }

    // Swapchain (single array swapchain, arraySize=2)
    bool CreateColorSwapchain(uint32_t width, uint32_t height, int64_t preferredFormat = 0);
    void DestroySwapchain();

    XrSwapchain GetSwapchain() const { return m_ColorSwapchain; }
    int64_t GetSwapchainFormat() const { return m_ColorSwapchainFormat; }
    uint32_t GetSwapchainWidth() const { return m_SwapchainWidth; }
    uint32_t GetSwapchainHeight() const { return m_SwapchainHeight; }
    const std::vector<VkImage>& GetSwapchainImages() const { return m_SwapchainVkImages; }

    // Frame loop
    bool BeginFrame(XRFrameInfo& outFrame);
    bool EndFrame(const XRFrameInfo& frame, const XrCompositionLayerBaseHeader* const* layers, uint32_t layerCount);

    // Views for stereo rendering
    bool LocateViews(XrViewConfigurationType viewType, XrTime displayTime, std::vector<XrView>& outViews);

    // Swapchain acquire/release
    bool AcquireSwapchainImage(uint32_t& outImageIndex);
    bool WaitSwapchainImage();
    bool ReleaseSwapchainImage();

    // Utility
    std::vector<int64_t> EnumerateSwapchainFormats() const;

private:
    // Function pointers (KHR_vulkan_enable2)
    PFN_xrGetVulkanInstanceExtensionsKHR  pfnGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR    pfnGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDevice2KHR     pfnGetVulkanGraphicsDevice2KHR = nullptr;

    bool LoadVulkanEnable2Functions();
    bool CreateSpaces();
    void DestroySpaces();

    static std::string XrResultString(XrInstance inst, XrResult r);

private:
    XrInstance   m_Instance = XR_NULL_HANDLE;
    XrSystemId   m_SystemId = XR_NULL_SYSTEM_ID;

    XrSession      m_Session = XR_NULL_HANDLE;
    XrSessionState m_State = XR_SESSION_STATE_UNKNOWN;
    bool           m_Running = false;
    bool           m_ExitRequested = false;

    // App reference space
    XrSpace      m_AppSpace = XR_NULL_HANDLE;

    // View configuration
    XrViewConfigurationType m_ViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    std::vector<XrView>     m_Views;

    // Swapchain (color)
    XrSwapchain  m_ColorSwapchain = XR_NULL_HANDLE;
    int64_t      m_ColorSwapchainFormat = 0;
    uint32_t     m_SwapchainWidth = 0;
    uint32_t     m_SwapchainHeight = 0;
    std::vector<VkImage> m_SwapchainVkImages;

    // Vulkan handles used by OpenXR binding
    VkInstance       m_VkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice         m_VkDevice = VK_NULL_HANDLE;
    uint32_t         m_GraphicsQueueFamily = 0;
    uint32_t         m_GraphicsQueueIndex = 0;
};
