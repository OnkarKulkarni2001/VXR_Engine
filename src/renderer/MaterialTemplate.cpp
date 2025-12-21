#include "MaterialTemplate.h"
#include "renderer/VulkanDevice.h"
#include "renderer/VulkanSwapchain.h"
#include "renderer/VulkanRenderPass.h"
#include "renderer/pipeline/VulkanPipeline.h"

MaterialTemplate::MaterialTemplate(
    VulkanDevice* device,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderPass,
    VkDescriptorSetLayout globalSetLayout,
    const std::string& vertSpv,
    const std::string& fragSpv)
    : m_Device(device)
{
    // --- Set 1 layout: albedo + normal ---
    VkDescriptorSetLayoutBinding albedo{};
    albedo.binding = 0;
    albedo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedo.descriptorCount = 1;
    albedo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding normal{};
    normal.binding = 1;
    normal.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normal.descriptorCount = 1;
    normal.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bindings[] = { albedo, normal };

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 2;
    info.pBindings = bindings;

    vkCreateDescriptorSetLayout(m_Device->GetHandle(), &info, nullptr, &m_MaterialSetLayout);

    // Pipeline uses set0 + set1
    std::vector<VkDescriptorSetLayout> setLayouts = { globalSetLayout, m_MaterialSetLayout };

    m_Pipeline = new VulkanPipeline(device, swapchain, renderPass, setLayouts, vertSpv, fragSpv);
}

MaterialTemplate::~MaterialTemplate()
{
    if (m_Pipeline) { delete m_Pipeline; m_Pipeline = nullptr; }

    if (m_MaterialSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_Device->GetHandle(), m_MaterialSetLayout, nullptr);
        m_MaterialSetLayout = VK_NULL_HANDLE;
    }
}
