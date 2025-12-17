#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class VulkanDevice;

class VulkanUniformBuffers
{
public:
    VulkanUniformBuffers(VulkanDevice* device, uint32_t framesInFlight, VkDeviceSize bufferSize);
    ~VulkanUniformBuffers();

    void Update(uint32_t frameIndex, const void* data, VkDeviceSize size);

    VkBuffer GetBuffer(uint32_t frameIndex) const { return m_Buffers[frameIndex]; }
    uint32_t GetCount() const { return static_cast<uint32_t>(m_Buffers.size()); }

private:
    VulkanDevice* m_Device = nullptr;
    VkDeviceSize  m_BufferSize = 0;

    std::vector<VkBuffer>       m_Buffers;
    std::vector<VkDeviceMemory> m_Memories;
    std::vector<void*>          m_Mapped; // mapped once, persistently
};
