#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <algorithm>

VulkanSwapchain::VulkanSwapchain(VkInstance instance, VulkanDevice* device, VkSurfaceKHR surface, uint32_t width, uint32_t height)
    : m_Device(device), m_Surface(surface)
{
    LOG_INFO("Creating Vulkan Swapchain...");

    CreateSwapchain(width, height);
    CreateImageViews();

    LOG_INFO("Swapchain successfully created!");
}

VulkanSwapchain::~VulkanSwapchain()
{
    for (auto view : m_ImageViews)
        vkDestroyImageView(m_Device->GetHandle(), view, nullptr);

    vkDestroySwapchainKHR(m_Device->GetHandle(), m_Swapchain, nullptr);
}

void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height)
{
    VkPhysicalDevice physical = m_Device->GetPhysicalDevice();

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, m_Surface, &caps);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, m_Surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, m_Surface, &formatCount, formats.data());

    uint32_t modeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, m_Surface, &modeCount, nullptr);

    std::vector<VkPresentModeKHR> modes(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, m_Surface, &modeCount, modes.data());

    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
    VkPresentModeKHR   presentMode = ChoosePresentMode(modes);
    VkExtent2D         extent = ChooseExtent(caps, width, height);

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
        imageCount = caps.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
    m_Device->GetGraphicsQueueFamilyIndex(),
    m_Device->GetPresentQueueFamilyIndex()
    };

    if (m_Device->GetGraphicsQueueFamilyIndex() != m_Device->GetPresentQueueFamilyIndex()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = caps.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_Device->GetHandle(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) {
        LOG_ERROR("Failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device->GetHandle(), m_Swapchain, &imageCount, m_Images.data());

    m_ImageFormat = surfaceFormat.format;
    m_Extent = extent;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& f : formats)
    {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return f;
    }
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes)
{
    for (auto m : modes)
        if (m == VK_PRESENT_MODE_MAILBOX_KHR)
            return m;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& caps, uint32_t width, uint32_t height)
{
    if (caps.currentExtent.width != UINT32_MAX)
        return caps.currentExtent;

    VkExtent2D actual = { width, height };

    actual.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, actual.width));
    actual.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, actual.height));

    return actual;
}

void VulkanSwapchain::CreateImageViews()
{
    m_ImageViews.resize(m_Images.size());

    for (size_t i = 0; i < m_Images.size(); i++)
    {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = m_Images[i];
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = m_ImageFormat;

        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Device->GetHandle(), &info, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create swapchain image view!");
        }
    }
}
