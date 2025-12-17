#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanDevice;

class VulkanCameraBuffers
{
public:
    VulkanCameraBuffers(VulkanDevice* device, uint32_t framesInFlight);
    ~VulkanCameraBuffers();

    VkBuffer GetBuffer(uint32_t frame) const;
    void* GetMapped(uint32_t frame) const;

private:
    VulkanDevice* m_Device = nullptr;

    std::vector<VkBuffer>       m_Buffers;
    std::vector<VkDeviceMemory> m_Memory;
    std::vector<void*>          m_Mapped;
};
