#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class VulkanDevice;
class VulkanUniformBuffers;

class VulkanDescriptors
{
public:
    VulkanDescriptors(VulkanDevice* device, VulkanUniformBuffers* ubo, uint32_t framesInFlight);
    ~VulkanDescriptors();

    VkDescriptorSetLayout GetLayout() const { return m_Layout; }
    VkDescriptorSet GetSet(uint32_t frameIndex) const { return m_Sets[frameIndex]; }

private:
    VulkanDevice* m_Device = nullptr;
    VulkanUniformBuffers* m_UBO = nullptr;

    VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
    VkDescriptorPool      m_Pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_Sets;
};
