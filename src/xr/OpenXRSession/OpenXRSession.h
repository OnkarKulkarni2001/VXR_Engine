#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

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
    void PollEvents();

    bool IsRunning() const { return m_Running; }
    XrSession GetSession() const { return m_Session; }
    XrSpace GetAppSpace() const { return m_AppSpace; }

    bool CreateColorSwapchain(uint32_t width, uint32_t height, int64_t preferredFormat = 0);
    void DestroySwapchain();

    XrSwapchain GetSwapchain() const { return m_ColorSwapchain; }
    int64_t GetSwapchainFormat() const { return m_ColorSwapchainFormat; }
    uint32_t GetSwapchainWidth() const { return m_SwapchainWidth; }
    uint32_t GetSwapchainHeight() const { return m_SwapchainHeight; }
    const std::vector<VkImage>& GetSwapchainImages() const { return m_SwapchainVkImages; }

    bool BeginFrame(XRFrameInfo& outFrame);
    bool EndFrame(const XRFrameInfo& frame, const XrCompositionLayerBaseHeader* const* layers, uint32_t layerCount);

    bool LocateViews(XrViewConfigurationType viewType, XrTime displayTime, std::vector<XrView>& outViews);

    bool AcquireSwapchainImage(uint32_t& outImageIndex);
    bool WaitSwapchainImage();
    bool ReleaseSwapchainImage();

    std::vector<int64_t> EnumerateSwapchainFormats() const;

private:
    bool LoadVulkanEnable2Functions();
    bool CreateSpaces();
    void DestroySpaces();

    static std::string XrResultString(XrInstance inst, XrResult r);

private:
    // KHR_vulkan_enable2 function pointers
    PFN_xrGetVulkanInstanceExtensionsKHR  pfnGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR    pfnGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDevice2KHR     pfnGetVulkanGraphicsDevice2KHR = nullptr;

private:
    XrInstance   m_Instance = XR_NULL_HANDLE;
    XrSystemId   m_SystemId = XR_NULL_SYSTEM_ID;

    XrSession      m_Session = XR_NULL_HANDLE;
    XrSessionState m_State = XR_SESSION_STATE_UNKNOWN;
    bool           m_Running = false;
    bool           m_ExitRequested = false;

    XrSpace m_AppSpace = XR_NULL_HANDLE;

    XrViewConfigurationType m_ViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    std::vector<XrView>     m_Views;

    XrSwapchain  m_ColorSwapchain = XR_NULL_HANDLE;
    int64_t      m_ColorSwapchainFormat = 0;
    uint32_t     m_SwapchainWidth = 0;
    uint32_t     m_SwapchainHeight = 0;
    std::vector<VkImage> m_SwapchainVkImages;

    VkInstance       m_VkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice         m_VkDevice = VK_NULL_HANDLE;
    uint32_t         m_GraphicsQueueFamily = 0;
    uint32_t         m_GraphicsQueueIndex = 0;
};
