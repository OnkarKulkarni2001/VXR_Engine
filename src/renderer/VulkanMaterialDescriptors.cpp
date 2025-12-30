#include "VulkanMaterialDescriptors.h"
#include "VulkanDevice.h"
#include "VulkanCheck.h"
#include "../core/Logger.h"

VulkanMaterialDescriptors::VulkanMaterialDescriptors(VulkanDevice* device, uint32_t maxMaterials)
    : m_Device(device)
{
    VkDevice vkDevice = m_Device->GetHandle();

	CreateLayout();

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
    if (!m_Device) return;
    VkDevice device = m_Device->GetHandle();

    if (m_Layout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, m_Layout, nullptr);
        m_Layout = VK_NULL_HANDLE;
    }

    if (m_Pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, m_Pool, nullptr);
        m_Pool = VK_NULL_HANDLE;
    }
}

void VulkanMaterialDescriptors::CreateLayout()
{
    VkDevice device = m_Device->GetHandle();

    VkDescriptorSetLayoutBinding bindings[2]{};

    // binding 0 → albedo
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // binding 1 → normal
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 2;
    info.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &m_Layout));
}
