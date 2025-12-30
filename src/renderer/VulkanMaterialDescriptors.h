#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class VulkanDevice;

class VulkanMaterialDescriptors
{
public:
    VulkanMaterialDescriptors(VulkanDevice* device, uint32_t maxMaterials);
    ~VulkanMaterialDescriptors();

    VkDescriptorPool GetPool() const { return m_Pool; }
    VkDescriptorSetLayout GetLayout() const { return m_Layout; }

private:
    void CreateLayout();

private:
    VulkanDevice* m_Device = nullptr;
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
};
