#include "VulkanDepthBuffer.h"
#include "VulkanDevice.h"
#include "../core/Logger.h"

VulkanDepthBuffer::VulkanDepthBuffer(VulkanDevice* device, VkExtent2D extent)
    : m_Device(device), m_Extent(extent)
{
    m_Format = m_Device->FindDepthFormat();

    VkSampleCountFlagBits samples = m_Device->GetMSAASamples();

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_Extent.width;
    imageInfo.extent.height = m_Extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_Format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_Device->GetHandle(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create depth image!");
        return;
    }

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(m_Device->GetHandle(), m_Image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_Device->FindMemoryType(
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    if (vkAllocateMemory(m_Device->GetHandle(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to allocate depth image memory!");
        return;
    }

    vkBindImageMemory(m_Device->GetHandle(), m_Image, m_Memory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_Format;

    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device->GetHandle(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create depth image view!");
    }
}

VulkanDepthBuffer::~VulkanDepthBuffer()
{
    if (m_ImageView)
        vkDestroyImageView(m_Device->GetHandle(), m_ImageView, nullptr);
    if (m_Image)
        vkDestroyImage(m_Device->GetHandle(), m_Image, nullptr);
    if (m_Memory)
        vkFreeMemory(m_Device->GetHandle(), m_Memory, nullptr);
}
