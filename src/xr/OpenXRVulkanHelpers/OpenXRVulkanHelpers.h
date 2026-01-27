#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// Returns OpenXR-required Vulkan instance/device extension name lists (as const char*).
// Pointers remain valid for lifetime of the program (static storage in .cpp).
void GetOpenXRVulkanExtensions(
    XrInstance xrInstance,
    XrSystemId systemId,
    std::vector<const char*>& outInstanceExts,
    std::vector<const char*>& outDeviceExts
);

// Ask OpenXR runtime for the VkPhysicalDevice it wants you to use (KHR_vulkan_enable2).
VkPhysicalDevice GetOpenXRVulkanGraphicsDevice(
    XrInstance xrInstance,
    XrSystemId systemId,
    VkInstance vkInstance
);
