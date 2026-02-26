#include "xr/OpenXRVulkanHelpers/OpenXRVulkanHelpers.h"

#include <string>
#include <vector>

// Splits OpenXR space-separated extension string into vector<string>
static std::vector<std::string> SplitSpaceSeparated(const char* str)
{
    std::vector<std::string> out;
    std::string s = str ? str : "";
    size_t start = 0;

    while (start < s.size())
    {
        while (start < s.size() && s[start] == ' ') start++;
        if (start >= s.size()) break;

        size_t end = s.find(' ', start);
        if (end == std::string::npos) end = s.size();

        out.push_back(s.substr(start, end - start));
        start = end + 1;
    }

    return out;
}

void GetOpenXRVulkanExtensions(
    XrInstance xrInstance,
    XrSystemId systemId,
    std::vector<const char*>& outInstanceExts,
    std::vector<const char*>& outDeviceExts
)
{
    // These entry points exist when XR_KHR_vulkan_enable2 is supported+enabled at xrCreateInstance.
    PFN_xrGetVulkanInstanceExtensionsKHR pfnGetInstExt = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR   pfnGetDevExt = nullptr;

    xrGetInstanceProcAddr(
        xrInstance,
        "xrGetVulkanInstanceExtensionsKHR",
        (PFN_xrVoidFunction*)&pfnGetInstExt
    );

    xrGetInstanceProcAddr(
        xrInstance,
        "xrGetVulkanDeviceExtensionsKHR",
        (PFN_xrVoidFunction*)&pfnGetDevExt
    );

    if (!pfnGetInstExt || !pfnGetDevExt)
    {
        outInstanceExts.clear();
        outDeviceExts.clear();
        return;
    }

    // Instance extensions string
    uint32_t instSize = 0;
    pfnGetInstExt(xrInstance, systemId, 0, &instSize, nullptr);
    std::string instStr(instSize ? instSize : 1, '\0');
    if (instSize)
        pfnGetInstExt(xrInstance, systemId, instSize, &instSize, instStr.data());

    // Device extensions string
    uint32_t devSize = 0;
    pfnGetDevExt(xrInstance, systemId, 0, &devSize, nullptr);
    std::string devStr(devSize ? devSize : 1, '\0');
    if (devSize)
        pfnGetDevExt(xrInstance, systemId, devSize, &devSize, devStr.data());

    // IMPORTANT: keep backing storage alive for returned const char*.
    static std::vector<std::string> instOwned;
    static std::vector<std::string> devOwned;
    instOwned = SplitSpaceSeparated(instStr.c_str());
    devOwned = SplitSpaceSeparated(devStr.c_str());

    outInstanceExts.clear();
    outDeviceExts.clear();

    for (auto& e : instOwned) outInstanceExts.push_back(e.c_str());
    for (auto& e : devOwned)  outDeviceExts.push_back(e.c_str());
}

VkPhysicalDevice GetOpenXRVulkanGraphicsDevice(
    XrInstance xrInstance,
    XrSystemId systemId,
    VkInstance vkInstance
)
{
    PFN_xrGetVulkanGraphicsDevice2KHR pfnGetGraphicsDevice2 = nullptr;

    xrGetInstanceProcAddr(
        xrInstance,
        "xrGetVulkanGraphicsDevice2KHR",
        (PFN_xrVoidFunction*)&pfnGetGraphicsDevice2
    );

    if (!pfnGetGraphicsDevice2)
        return VK_NULL_HANDLE;

    // XR_KHR_vulkan_enable2
    XrVulkanGraphicsDeviceGetInfoKHR info{ XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR };
    info.systemId = systemId;
    info.vulkanInstance = vkInstance;

    VkPhysicalDevice phys = VK_NULL_HANDLE;
    const XrResult r = pfnGetGraphicsDevice2(xrInstance, &info, &phys);
    if (XR_FAILED(r))
        return VK_NULL_HANDLE;

    return phys;
}
