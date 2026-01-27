#include "OpenXRInstance.h"
#include "core/Logger.h"

#include <vector>
#include <cstring>

static const char* XrResultToString(XrInstance inst, XrResult r)
{
    static char buf[XR_MAX_RESULT_STRING_SIZE];
    if (inst != XR_NULL_HANDLE)
    {
        xrResultToString(inst, r, buf);
        return buf;
    }
    return "XR error";
}

#define XR_CHECK(inst, expr)                                                     \
    do {                                                                         \
        XrResult _r = (expr);                                                    \
        if (XR_FAILED(_r)) {                                                     \
            LOG_ERROR(std::string("OpenXR call failed: ") + #expr + " -> " +     \
                      XrResultToString(inst, _r));                               \
        }                                                                        \
    } while (0)

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
        m_SystemId = XR_NULL_SYSTEM_ID;
    }
}

void OpenXRInstance::CreateInstance()
{
    XrInstanceCreateInfo ci{ XR_TYPE_INSTANCE_CREATE_INFO };
    std::strncpy(ci.applicationInfo.applicationName, "VXR_Engine", XR_MAX_APPLICATION_NAME_SIZE);
    ci.applicationInfo.applicationVersion = 1;
    std::strncpy(ci.applicationInfo.engineName, "VXR", XR_MAX_ENGINE_NAME_SIZE);
    ci.applicationInfo.engineVersion = 1;
    ci.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    // Minimal extensions for now (no Vulkan binding yet)
    std::vector<const char*> exts;

#ifdef _DEBUG
    // Optional: debug utils (only if runtime supports it; we'll keep it simple for now)
    // exts.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    ci.enabledExtensionCount = (uint32_t)exts.size();
    ci.enabledExtensionNames = exts.empty() ? nullptr : exts.data();

    XrResult r = xrCreateInstance(&ci, &m_Instance);
    if (XR_FAILED(r))
    {
        LOG_ERROR(std::string("xrCreateInstance failed: ") + XrResultToString(XR_NULL_HANDLE, r));
        m_Instance = XR_NULL_HANDLE;
        return;
    }

    LOG_INFO("OpenXR instance created.");
}

void OpenXRInstance::GetSystem()
{
    if (m_Instance == XR_NULL_HANDLE) return;

    XrSystemGetInfo si{ XR_TYPE_SYSTEM_GET_INFO };
    si.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrResult r = xrGetSystem(m_Instance, &si, &m_SystemId);
    if (XR_FAILED(r))
    {
        LOG_ERROR(std::string("xrGetSystem failed: ") + XrResultToString(m_Instance, r));
        m_SystemId = XR_NULL_SYSTEM_ID;
        return;
    }

    XrSystemProperties props{ XR_TYPE_SYSTEM_PROPERTIES };
    XR_CHECK(m_Instance, xrGetSystemProperties(m_Instance, m_SystemId, &props));

    LOG_INFO(std::string("OpenXR system: ") + props.systemName);
}
