#include "VulkanFramebuffers.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDepthBuffer.h"
#include "VulkanMSAAColorBuffer.h"
#include "VulkanRenderPass.h"
#include "../core/Logger.h"

VulkanFramebuffers::VulkanFramebuffers(VulkanDevice* device,
    VulkanSwapchain* swapchain,
    VulkanRenderPass* renderPass,
    VulkanDepthBuffer* depthBuffer,
    VulkanMSAAColorBuffer* msaaColor)
    : m_Device(device),
    m_Swapchain(swapchain),
    m_RenderPass(renderPass),
    m_DepthBuffer(depthBuffer),
    m_MSAAColor(msaaColor)
{
    const auto& swapViews = m_Swapchain->GetImageViews();
    m_Framebuffers.resize(swapViews.size());

    for (size_t i = 0; i < swapViews.size(); ++i)
    {
        VkImageView attachments[3] = {
            m_MSAAColor->GetImageView(),
            m_DepthBuffer->GetImageView(),
            swapViews[i]
        };

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = m_RenderPass->GetHandle();
        fbInfo.attachmentCount = 3;
        fbInfo.pAttachments = attachments;
        fbInfo.width = m_Swapchain->GetExtent().width;
        fbInfo.height = m_Swapchain->GetExtent().height;
        fbInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device->GetHandle(), &fbInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create framebuffer!");
        }
    }
}

VulkanFramebuffers::~VulkanFramebuffers()
{
    for (auto fb : m_Framebuffers)
        vkDestroyFramebuffer(m_Device->GetHandle(), fb, nullptr);
}
