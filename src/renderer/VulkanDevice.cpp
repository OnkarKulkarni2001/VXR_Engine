#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <set>
#include <iostream>

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
{
    PickPhysicalDevice(instance, surface);
    DetermineMSAASamples();       // NEW: decide what MSAA we can use
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

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueInfos.size();
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();


    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create logical device!");
        return;
    }

    vkGetDeviceQueue(m_Device, m_GraphicsFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentFamilyIndex, 0, &m_PresentQueue);

    LOG_INFO("Logical device created successfully!");
}

void VulkanDevice::DetermineMSAASamples()
{
    m_MSAASamples = GetMaxUsableSampleCount();

    LOG_INFO("Using MSAA sample count: " + std::to_string(static_cast<int>(m_MSAASamples)));
}

VkSampleCountFlagBits VulkanDevice::GetMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);

    VkSampleCountFlags colorCounts = props.limits.framebufferColorSampleCounts;
    VkSampleCountFlags depthCounts = props.limits.framebufferDepthSampleCounts;

    VkSampleCountFlags counts = colorCounts & depthCounts;

    if (counts & VK_SAMPLE_COUNT_8_BIT)  return VK_SAMPLE_COUNT_8_BIT;
    if (counts & VK_SAMPLE_COUNT_4_BIT)  return VK_SAMPLE_COUNT_4_BIT;
    if (counts & VK_SAMPLE_COUNT_2_BIT)  return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

VkFormat VulkanDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    LOG_ERROR("Failed to find supported format!");
    return VK_FORMAT_UNDEFINED;
}

VkFormat VulkanDevice::FindDepthFormat() const
{
    return FindSupportedFormat(
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    LOG_ERROR("Failed to find suitable memory type!");
    return 0;
}