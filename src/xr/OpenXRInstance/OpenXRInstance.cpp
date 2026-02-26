#include "xr/OpenXRInstance/OpenXRInstance.h"
#include "core/Logger.h"

#include <vector>
#include <cstring>

OpenXRInstance::OpenXRInstance()
{
    CreateInstance();
    GetSystem();
}

OpenXRInstance::~OpenXRInstance()
{
    if (m_Instance != XR_NULL_HANDLE)
    {
        xrDestroyInstance(m_Instance);
        m_Instance = XR_NULL_HANDLE;
    }
}

void OpenXRInstance::CreateInstance()
{
    // Required extension for Vulkan binding via XR_KHR_vulkan_enable2.
    // If your runtime doesn't support enable2, you can fallback to XR_KHR_vulkan_enable.
    std::vector<const char*> enabledExtensions;
    enabledExtensions.push_back(XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME);

#ifdef _DEBUG
    // If you later enable XR_EXT_debug_utils, add it here and create a messenger.
    // enabledExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
    std::strncpy(createInfo.applicationInfo.applicationName, "VXR Engine", XR_MAX_APPLICATION_NAME_SIZE - 1);
    createInfo.applicationInfo.applicationVersion = 1;
    std::strncpy(createInfo.applicationInfo.engineName, "VXR", XR_MAX_ENGINE_NAME_SIZE - 1);
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    createInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
    createInfo.enabledExtensionNames = enabledExtensions.data();

    XrResult r = xrCreateInstance(&createInfo, &m_Instance);
    if (XR_FAILED(r))
    {
        LOG_ERROR("Failed to create OpenXR instance!");
        m_Instance = XR_NULL_HANDLE;
        return;
    }

    LOG_INFO("OpenXR instance created.");
}

void OpenXRInstance::GetSystem()
{
    if (m_Instance == XR_NULL_HANDLE)
        return;

    XrSystemGetInfo sysInfo{ XR_TYPE_SYSTEM_GET_INFO };
    sysInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrResult r = xrGetSystem(m_Instance, &sysInfo, &m_SystemId);
    if (XR_FAILED(r))
    {
        LOG_ERROR("Failed to get OpenXR system!");
        m_SystemId = XR_NULL_SYSTEM_ID;
        return;
    }

    LOG_INFO("OpenXR system acquired.");
}

void OpenXRInstance::CreateDebugMessenger()
{
    // Optional; leave empty for now.
}
