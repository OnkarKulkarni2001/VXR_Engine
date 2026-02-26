#include "xr/OpenXRSession/OpenXRSession.h"
#include "core/Logger.h"

#include <cstring>

static bool XR_SUCCEEDED_LOG(XrInstance inst, XrResult r, const char* what)
{
    if (XR_FAILED(r))
    {
        char buf[XR_MAX_RESULT_STRING_SIZE]{};
        if (inst != XR_NULL_HANDLE) xrResultToString(inst, r, buf);
        LOG_ERROR(std::string("[OpenXR] ") + what + " failed: " + (buf[0] ? buf : "XR error"));
        return false;
    }
    return true;
}

std::string OpenXRSession::XrResultString(XrInstance inst, XrResult r)
{
    char buf[XR_MAX_RESULT_STRING_SIZE]{};
    if (inst != XR_NULL_HANDLE) xrResultToString(inst, r, buf);
    return buf[0] ? std::string(buf) : std::string("XR error");
}

OpenXRSession::~OpenXRSession()
{
    Destroy();
}

bool OpenXRSession::LoadVulkanEnable2Functions()
{
    auto load = [&](const char* name, void** fn) -> bool
        {
            XrResult r = xrGetInstanceProcAddr(m_Instance, name, (PFN_xrVoidFunction*)fn);
            return XR_SUCCEEDED_LOG(m_Instance, r, name);
        };

    if (!load("xrGetVulkanInstanceExtensionsKHR", (void**)&pfnGetVulkanInstanceExtensionsKHR)) return false;
    if (!load("xrGetVulkanDeviceExtensionsKHR", (void**)&pfnGetVulkanDeviceExtensionsKHR))   return false;
    if (!load("xrGetVulkanGraphicsDevice2KHR", (void**)&pfnGetVulkanGraphicsDevice2KHR))    return false;

    return true;
}

bool OpenXRSession::Create(
    XrInstance instance,
    XrSystemId systemId,
    VkInstance vkInstance,
    VkPhysicalDevice vkPhysicalDevice,
    VkDevice vkDevice,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t graphicsQueueIndex
)
{
    m_Instance = instance;
    m_SystemId = systemId;

    m_VkInstance = vkInstance;
    m_VkPhysicalDevice = vkPhysicalDevice;
    m_VkDevice = vkDevice;
    m_GraphicsQueueFamily = graphicsQueueFamilyIndex;
    m_GraphicsQueueIndex = graphicsQueueIndex;

    if (m_Instance == XR_NULL_HANDLE || m_SystemId == XR_NULL_SYSTEM_ID)
    {
        LOG_ERROR("[OpenXRSession] Invalid instance/system.");
        return false;
    }

    if (!LoadVulkanEnable2Functions())
    {
        LOG_ERROR("[OpenXRSession] Failed to load Vulkan enable2 functions. Ensure XR_KHR_vulkan_enable2 is enabled at xrCreateInstance.");
        return false;
    }

    XrGraphicsBindingVulkan2KHR bind{ XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR };
    bind.instance = m_VkInstance;
    bind.physicalDevice = m_VkPhysicalDevice;
    bind.device = m_VkDevice;
    bind.queueFamilyIndex = m_GraphicsQueueFamily;
    bind.queueIndex = m_GraphicsQueueIndex;

    XrSessionCreateInfo sci{ XR_TYPE_SESSION_CREATE_INFO };
    sci.next = &bind;
    sci.systemId = m_SystemId;

    const XrResult r = xrCreateSession(m_Instance, &sci, &m_Session);
    if (!XR_SUCCEEDED_LOG(m_Instance, r, "xrCreateSession"))
    {
        m_Session = XR_NULL_HANDLE;
        return false;
    }

    LOG_INFO("[OpenXR] Session created.");

    if (!CreateSpaces())
        return false;

    // Pre-allocate views (usually 2 for stereo)
    m_Views.resize(2);
    for (auto& v : m_Views) v.type = XR_TYPE_VIEW;

    return true;
}

void OpenXRSession::Destroy()
{
    DestroySwapchain();
    DestroySpaces();

    if (m_Session != XR_NULL_HANDLE)
    {
        xrDestroySession(m_Session);
        m_Session = XR_NULL_HANDLE;
    }

    m_Running = false;
    m_ExitRequested = false;
    m_State = XR_SESSION_STATE_UNKNOWN;

    m_Instance = XR_NULL_HANDLE;
    m_SystemId = XR_NULL_SYSTEM_ID;
}

bool OpenXRSession::CreateSpaces()
{
    XrReferenceSpaceCreateInfo rs{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    rs.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    rs.poseInReferenceSpace.orientation = { 0,0,0,1 };
    rs.poseInReferenceSpace.position = { 0,0,0 };

    const XrResult r = xrCreateReferenceSpace(m_Session, &rs, &m_AppSpace);
    if (!XR_SUCCEEDED_LOG(m_Instance, r, "xrCreateReferenceSpace(LOCAL)"))
    {
        m_AppSpace = XR_NULL_HANDLE;
        return false;
    }

    LOG_INFO("[OpenXR] Reference space created (LOCAL).");
    return true;
}

void OpenXRSession::DestroySpaces()
{
    if (m_AppSpace != XR_NULL_HANDLE)
    {
        xrDestroySpace(m_AppSpace);
        m_AppSpace = XR_NULL_HANDLE;
    }
}

void OpenXRSession::PollEvents()
{
    if (m_Instance == XR_NULL_HANDLE) return;

    XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };
    while (xrPollEvent(m_Instance, &event) == XR_SUCCESS)
    {
        switch (event.type)
        {
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
        {
            auto* sc = (XrEventDataSessionStateChanged*)&event;
            m_State = sc->state;

            if (m_State == XR_SESSION_STATE_READY)
            {
                XrSessionBeginInfo bi{ XR_TYPE_SESSION_BEGIN_INFO };
                bi.primaryViewConfigurationType = m_ViewType;

                const XrResult r = xrBeginSession(m_Session, &bi);
                if (XR_SUCCEEDED_LOG(m_Instance, r, "xrBeginSession"))
                {
                    m_Running = true;
                    LOG_INFO("[OpenXR] Session running.");
                }
            }
            else if (m_State == XR_SESSION_STATE_STOPPING)
            {
                const XrResult r = xrEndSession(m_Session);
                XR_SUCCEEDED_LOG(m_Instance, r, "xrEndSession");
                m_Running = false;
                LOG_INFO("[OpenXR] Session stopped.");
            }
            else if (m_State == XR_SESSION_STATE_EXITING || m_State == XR_SESSION_STATE_LOSS_PENDING)
            {
                m_ExitRequested = true;
                m_Running = false;
                LOG_INFO("[OpenXR] Exit requested.");
            }
        } break;

        default:
            break;
        }

        event = { XR_TYPE_EVENT_DATA_BUFFER };
    }
}

std::vector<int64_t> OpenXRSession::EnumerateSwapchainFormats() const
{
    std::vector<int64_t> formats;
    if (m_Session == XR_NULL_HANDLE) return formats;

    uint32_t count = 0;
    xrEnumerateSwapchainFormats(m_Session, 0, &count, nullptr);
    formats.resize(count);
    xrEnumerateSwapchainFormats(m_Session, count, &count, formats.data());

    return formats;
}

bool OpenXRSession::CreateColorSwapchain(uint32_t width, uint32_t height, int64_t preferredFormat)
{
    DestroySwapchain();

    if (m_Session == XR_NULL_HANDLE)
    {
        LOG_ERROR("[OpenXR] CreateColorSwapchain called without session.");
        return false;
    }

    const auto formats = EnumerateSwapchainFormats();
    if (formats.empty())
    {
        LOG_ERROR("[OpenXR] No swapchain formats reported.");
        return false;
    }

    auto supported = [&](int64_t fmt) {
        for (auto f : formats) if (f == fmt) return true;
        return false;
        };

    int64_t chosen = 0;
    if (preferredFormat != 0 && supported(preferredFormat))
    {
        chosen = preferredFormat;
    }
    else
    {
        const int64_t tryList[] = {
            (int64_t)VK_FORMAT_R8G8B8A8_SRGB,
            (int64_t)VK_FORMAT_B8G8R8A8_SRGB,
            (int64_t)VK_FORMAT_R8G8B8A8_UNORM,
            (int64_t)VK_FORMAT_B8G8R8A8_UNORM
        };

        for (auto f : tryList)
        {
            if (supported(f)) { chosen = f; break; }
        }

        if (chosen == 0)
            chosen = formats[0];
    }

    XrSwapchainCreateInfo ci{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
    ci.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
    ci.format = chosen;
    ci.sampleCount = 1;
    ci.width = width;
    ci.height = height;
    ci.faceCount = 1;
    ci.arraySize = 2; // stereo
    ci.mipCount = 1;

    XrResult r = xrCreateSwapchain(m_Session, &ci, &m_ColorSwapchain);
    if (!XR_SUCCEEDED_LOG(m_Instance, r, "xrCreateSwapchain(color)"))
    {
        m_ColorSwapchain = XR_NULL_HANDLE;
        return false;
    }

    m_ColorSwapchainFormat = chosen;
    m_SwapchainWidth = width;
    m_SwapchainHeight = height;

    // Enumerate swapchain images -> VkImage list
    uint32_t imageCount = 0;
    xrEnumerateSwapchainImages(m_ColorSwapchain, 0, &imageCount, nullptr);

    std::vector<XrSwapchainImageVulkanKHR> xrImages(imageCount);
    for (auto& im : xrImages) im.type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR;

    r = xrEnumerateSwapchainImages(
        m_ColorSwapchain,
        imageCount,
        &imageCount,
        (XrSwapchainImageBaseHeader*)xrImages.data()
    );

    if (!XR_SUCCEEDED_LOG(m_Instance, r, "xrEnumerateSwapchainImages"))
        return false;

    m_SwapchainVkImages.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i)
        m_SwapchainVkImages[i] = xrImages[i].image;

    LOG_INFO("[OpenXR] Color swapchain created. Images: " + std::to_string(imageCount) +
        " Format(Vk): " + std::to_string((int)chosen) +
        " Size: " + std::to_string(width) + "x" + std::to_string(height));

    return true;
}

void OpenXRSession::DestroySwapchain()
{
    m_SwapchainVkImages.clear();

    if (m_ColorSwapchain != XR_NULL_HANDLE)
    {
        xrDestroySwapchain(m_ColorSwapchain);
        m_ColorSwapchain = XR_NULL_HANDLE;
    }

    m_ColorSwapchainFormat = 0;
    m_SwapchainWidth = 0;
    m_SwapchainHeight = 0;
}

bool OpenXRSession::BeginFrame(XRFrameInfo& outFrame)
{
    if (!m_Running) return false;

    XrFrameWaitInfo wi{ XR_TYPE_FRAME_WAIT_INFO };
    XrFrameState fs{ XR_TYPE_FRAME_STATE };

    if (!XR_SUCCEEDED_LOG(m_Instance, xrWaitFrame(m_Session, &wi, &fs), "xrWaitFrame"))
        return false;

    XrFrameBeginInfo bi{ XR_TYPE_FRAME_BEGIN_INFO };
    if (!XR_SUCCEEDED_LOG(m_Instance, xrBeginFrame(m_Session, &bi), "xrBeginFrame"))
        return false;

    outFrame.predictedDisplayTime = fs.predictedDisplayTime;
    outFrame.predictedDisplayPeriod = fs.predictedDisplayPeriod;
    outFrame.shouldRender = (fs.shouldRender == XR_TRUE);

    return true;
}

bool OpenXRSession::EndFrame(const XRFrameInfo& frame, const XrCompositionLayerBaseHeader* const* layers, uint32_t layerCount)
{
    if (!m_Running) return false;

    XrFrameEndInfo ei{ XR_TYPE_FRAME_END_INFO };
    ei.displayTime = frame.predictedDisplayTime;
    ei.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    ei.layerCount = layerCount;
    ei.layers = layers;

    return XR_SUCCEEDED_LOG(m_Instance, xrEndFrame(m_Session, &ei), "xrEndFrame");
}

bool OpenXRSession::LocateViews(XrViewConfigurationType viewType, XrTime displayTime, std::vector<XrView>& outViews)
{
    if (!m_Running) return false;

    uint32_t viewCount = 0;
    xrEnumerateViewConfigurationViews(m_Instance, m_SystemId, viewType, 0, &viewCount, nullptr);
    if (viewCount == 0) return false;

    outViews.resize(viewCount);
    for (auto& v : outViews) v.type = XR_TYPE_VIEW;

    XrViewLocateInfo li{ XR_TYPE_VIEW_LOCATE_INFO };
    li.viewConfigurationType = viewType;
    li.displayTime = displayTime;
    li.space = m_AppSpace;

    XrViewState vs{ XR_TYPE_VIEW_STATE };
    uint32_t outCount = 0;

    const XrResult r = xrLocateViews(m_Session, &li, &vs, viewCount, &outCount, outViews.data());
    if (!XR_SUCCEEDED_LOG(m_Instance, r, "xrLocateViews"))
        return false;

    return true;
}

bool OpenXRSession::AcquireSwapchainImage(uint32_t& outImageIndex)
{
    if (m_ColorSwapchain == XR_NULL_HANDLE) return false;

    XrSwapchainImageAcquireInfo ai{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
    uint32_t idx = 0;

    if (!XR_SUCCEEDED_LOG(m_Instance, xrAcquireSwapchainImage(m_ColorSwapchain, &ai, &idx), "xrAcquireSwapchainImage"))
        return false;

    outImageIndex = idx;
    return true;
}

bool OpenXRSession::WaitSwapchainImage()
{
    if (m_ColorSwapchain == XR_NULL_HANDLE) return false;

    XrSwapchainImageWaitInfo wi{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
    wi.timeout = XR_INFINITE_DURATION;

    return XR_SUCCEEDED_LOG(m_Instance, xrWaitSwapchainImage(m_ColorSwapchain, &wi), "xrWaitSwapchainImage");
}

bool OpenXRSession::ReleaseSwapchainImage()
{
    if (m_ColorSwapchain == XR_NULL_HANDLE) return false;

    XrSwapchainImageReleaseInfo ri{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
    return XR_SUCCEEDED_LOG(m_Instance, xrReleaseSwapchainImage(m_ColorSwapchain, &ri), "xrReleaseSwapchainImage");
}
