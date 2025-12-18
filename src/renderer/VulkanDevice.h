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

    uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex() const { return m_PresentFamilyIndex; }

    // NEW:
    VkSampleCountFlagBits GetMSAASamples() const { return m_MSAASamples; }

    VkFormat FindDepthFormat() const;

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    void CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory
    );

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    // --- One-time command helpers (for copy/transition) ---
    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;


private:
    void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    int  RateDevice(VkPhysicalDevice device);

    void CreateLogicalDevice(VkSurfaceKHR surface);
    void FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    void DetermineMSAASamples();
    VkSampleCountFlagBits GetMaxUsableSampleCount();

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) const;

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice         m_Device = VK_NULL_HANDLE;

    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkCommandPool m_TransferCommandPool = VK_NULL_HANDLE;

    uint32_t m_GraphicsFamilyIndex = UINT32_MAX;
    uint32_t m_PresentFamilyIndex = UINT32_MAX;

    // NEW: MSAA sample count
    VkSampleCountFlagBits m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;

};
