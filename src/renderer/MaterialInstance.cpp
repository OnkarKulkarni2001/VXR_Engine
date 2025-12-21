#include "MaterialInstance.h"
#include "MaterialTemplate.h"
#include "VulkanMaterialDescriptors.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

#include <array>

MaterialInstance::MaterialInstance(
    VulkanDevice* device,
    VulkanMaterialDescriptors* materialPool,
    MaterialTemplate* templ,
    VkImageView albedoView, VkSampler albedoSampler,
    VkImageView normalView, VkSampler normalSampler)
    : m_Device(device), m_Template(templ)
{
    VkDevice vkDevice = m_Device->GetHandle();

    // Allocate 1 descriptor set using template's set=1 layout
    VkDescriptorSetLayout layout = m_Template->GetMaterialSetLayout();

    VkDescriptorSetAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc.descriptorPool = materialPool->GetPool();
    alloc.descriptorSetCount = 1;
    alloc.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(vkDevice, &alloc, &m_Set1) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to allocate material descriptor set (set=1)!");
        m_Set1 = VK_NULL_HANDLE;
        return;
    }

    VkDescriptorImageInfo albedoInfo{};
    albedoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    albedoInfo.imageView = albedoView;
    albedoInfo.sampler = albedoSampler;

    VkDescriptorImageInfo normalInfo{};
    normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    normalInfo.imageView = normalView;
    normalInfo.sampler = normalSampler;

    std::array<VkWriteDescriptorSet, 2> writes{};

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = m_Set1;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[0].descriptorCount = 1;
    writes[0].pImageInfo = &albedoInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = m_Set1;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &normalInfo;

    vkUpdateDescriptorSets(vkDevice, (uint32_t)writes.size(), writes.data(), 0, nullptr);

    LOG_INFO("MaterialInstance created (set=1: albedo + normal).");
}

VulkanPipeline* MaterialInstance::GetPipeline() const
{
    return m_Template ? m_Template->GetPipeline() : nullptr;
}
