#include "VulkanDescriptors.h"
#include "VulkanDevice.h"
#include "VulkanUniformBuffers.h"
#include "../core/Logger.h"

VulkanDescriptors::VulkanDescriptors(VulkanDevice* device, VulkanUniformBuffers* ubo, uint32_t framesInFlight)
    : m_Device(device), m_UBO(ubo)
{
    VkDevice vkDevice = m_Device->GetHandle();

    // 1) Layout: set=0 binding=0 uniform buffer
    VkDescriptorSetLayoutBinding uboBinding{};
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;


    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboBinding;

    if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_Layout) != VK_SUCCESS)
        LOG_ERROR("Failed to create descriptor set layout!");

    // 2) Pool (one UBO descriptor per frame)
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = framesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = framesInFlight;

    if (vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &m_Pool) != VK_SUCCESS)
        LOG_ERROR("Failed to create descriptor pool!");

    // 3) Allocate sets (one per frame)
    std::vector<VkDescriptorSetLayout> layouts(framesInFlight, m_Layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_Pool;
    allocInfo.descriptorSetCount = framesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    m_Sets.resize(framesInFlight);
    if (vkAllocateDescriptorSets(vkDevice, &allocInfo, m_Sets.data()) != VK_SUCCESS)
        LOG_ERROR("Failed to allocate descriptor sets!");

    // 4) Write each set to point to that frame's UBO buffer
    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UBO->GetBuffer(i);
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_Sets[i];
        write.dstBinding = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(vkDevice, 1, &write, 0, nullptr);
    }

    LOG_INFO("Descriptor layout + per-frame descriptor sets created.");
}

VulkanDescriptors::~VulkanDescriptors()
{
    VkDevice vkDevice = m_Device->GetHandle();

    if (m_Pool) vkDestroyDescriptorPool(vkDevice, m_Pool, nullptr);
    if (m_Layout) vkDestroyDescriptorSetLayout(vkDevice, m_Layout, nullptr);
}
