#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <set>
#include <iostream>

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
{
    PickPhysicalDevice(instance, surface);
    CreateLogicalDevice(surface);
}

VulkanDevice::~VulkanDevice()
{
    if (m_Device)
        vkDestroyDevice(m_Device, nullptr);
}

void VulkanDevice::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOG_ERROR("No Vulkan-capable GPU found!");
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    int bestScore = -1;

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, surface))
        {
            int score = RateDevice(device);

            if (score > bestScore)
            {
                bestScore = score;
                m_PhysicalDevice = device;
            }
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE)
    {
        LOG_ERROR("Failed: No suitable GPU found!");
    }
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    FindQueueFamilies(device, surface);

    return m_GraphicsFamilyIndex != UINT32_MAX &&
        m_PresentFamilyIndex != UINT32_MAX;
}

int VulkanDevice::RateDevice(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    int score = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    score += props.limits.maxImageDimension2D;

    return score;
}

void VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queues.data());

    for (uint32_t i = 0; i < queueCount; i++)
    {
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            m_GraphicsFamilyIndex = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport)
            m_PresentFamilyIndex = i;

        if (m_GraphicsFamilyIndex != UINT32_MAX &&
            m_PresentFamilyIndex != UINT32_MAX)
        {
            return;
        }
    }
}

void VulkanDevice::CreateLogicalDevice(VkSurfaceKHR surface)
{
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::set<uint32_t> uniqueQueues = {
        m_GraphicsFamilyIndex,
        m_PresentFamilyIndex
    };

    float queuePriority = 1.0f;

    for (uint32_t queue : uniqueQueues)
    {
        VkDeviceQueueCreateInfo qInfo{};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = queue;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &queuePriority;

        queueInfos.push_back(qInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueInfos.size();
    createInfo.pQueueCreateInfos = queueInfos.data();

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create logical device!");
        return;
    }

    vkGetDeviceQueue(m_Device, m_GraphicsFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentFamilyIndex, 0, &m_PresentQueue);

    LOG_INFO("Logical device created successfully!");
}
