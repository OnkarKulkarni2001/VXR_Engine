#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice
{
public:
    VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
    ~VulkanDevice();

    VkDevice GetHandle() const { return m_Device; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }

private:
    void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    int  RateDevice(VkPhysicalDevice device);

    void CreateLogicalDevice(VkSurfaceKHR surface);
    void FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice         m_Device = VK_NULL_HANDLE;

    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;

    uint32_t m_GraphicsFamilyIndex = UINT32_MAX;
    uint32_t m_PresentFamilyIndex = UINT32_MAX;
};
