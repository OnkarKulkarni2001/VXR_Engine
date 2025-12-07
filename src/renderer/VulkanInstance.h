#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanInstance
{
public:
    VulkanInstance(bool enableValidationLayers = true);
    ~VulkanInstance();

    VkInstance GetHandle() const { return m_Instance; }

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    bool m_EnableValidationLayers = true;

private:
    void CreateInstance();
    void SetupDebugMessenger();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();

    // Debug utils loader functions
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
};
