#include "VulkanMaterialDescriptors.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

VulkanMaterialDescriptors::VulkanMaterialDescriptors(VulkanDevice* device, uint32_t maxMaterials)
    : m_Device(device)
{
    VkDevice vkDevice = m_Device->GetHandle();

    // We need COMBINED_IMAGE_SAMPLER descriptors for:
    // binding 0 = albedo, binding 1 = normal
    // => 2 per material
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = maxMaterials * 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxMaterials;

    if (vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &m_Pool) != VK_SUCCESS)
        LOG_ERROR("Failed to create material descriptor pool!");
    else
        LOG_INFO("Material descriptor pool created.");
}

VulkanMaterialDescriptors::~VulkanMaterialDescriptors()
{
    VkDevice vkDevice = m_Device->GetHandle();
    if (m_Pool)
        vkDestroyDescriptorPool(vkDevice, m_Pool, nullptr);
}
