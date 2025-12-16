#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;

class VulkanSwapchain
{
public:
    VulkanSwapchain(VkInstance instance, VulkanDevice* device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    ~VulkanSwapchain();

    VkSwapchainKHR GetHandle() const { return m_Swapchain; }
    VkFormat GetImageFormat() const { return m_ImageFormat; }
    VkExtent2D GetExtent() const { return m_Extent; }
    const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }

private:
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VulkanDevice* m_Device = nullptr;
    VkSurfaceKHR   m_Surface = VK_NULL_HANDLE;

    std::vector<VkImage>     m_Images;
    std::vector<VkImageView> m_ImageViews;

    VkFormat   m_ImageFormat;
    VkExtent2D m_Extent;

private:
    void CreateSwapchain(uint32_t width, uint32_t height);
    void CreateImageViews();

    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR   ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D         ChooseExtent(const VkSurfaceCapabilitiesKHR& caps, uint32_t width, uint32_t height);
};
