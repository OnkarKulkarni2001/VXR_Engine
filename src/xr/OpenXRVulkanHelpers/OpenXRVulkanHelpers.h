#pragma once

#include "xr/OpenXRCommon.h"

#include <vector>

// Returns OpenXR-required Vulkan instance/device extension name lists (as const char*).
// The returned const char* pointers remain valid for the lifetime of the program.
void GetOpenXRVulkanExtensions(
    XrInstance xrInstance,
    XrSystemId systemId,
    std::vector<const char*>& outInstanceExts,
    std::vector<const char*>& outDeviceExts
);

// Ask OpenXR runtime for the VkPhysicalDevice it wants you to use.
VkPhysicalDevice GetOpenXRVulkanGraphicsDevice(
    XrInstance xrInstance,
    XrSystemId systemId,
    VkInstance vkInstance
);
