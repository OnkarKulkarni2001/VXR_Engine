#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>

class VulkanDevice;
class VulkanCommandPool;

class VulkanTexture2D
{
public:
    VulkanTexture2D(
        VulkanDevice* device,
        VulkanCommandPool* cmdPool,
        const void* rgbaPixels,
        uint32_t width,
        uint32_t height);

    ~VulkanTexture2D();

    VkImageView GetView() const { return m_View; }
    VkSampler   GetSampler() const { return m_Sampler; }

private:
    void CreateImage(uint32_t w, uint32_t h);
    void CreateView();
    void CreateSampler();
    void Upload(const void* rgbaPixels, uint32_t w, uint32_t h);

private:
    VulkanDevice* m_Device = nullptr;
    VulkanCommandPool* m_CmdPool = nullptr;

    VkImage        m_Image = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkImageView    m_View = VK_NULL_HANDLE;
    VkSampler      m_Sampler = VK_NULL_HANDLE;

    VkFormat m_Format = VK_FORMAT_R8G8B8A8_UNORM;
};
