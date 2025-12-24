#include "VulkanTexture2D.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "../core/Logger.h"

#include <stdexcept>
#include <cstring>

// ---- Helpers you likely already have somewhere.
// If you DON'T, tell me what helpers exist in VulkanDevice / CommandPool and I’ll adapt.
// For now we assume VulkanDevice has: FindMemoryType(..)
// and VulkanCommandPool has: BeginSingleTimeCommands() / EndSingleTimeCommands(cmd)
// If you don’t, you can implement them quickly (I can paste those too).

static void CreateBuffer(
    VulkanDevice* device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags props,
    VkBuffer& outBuffer,
    VkDeviceMemory& outMemory)
{
    VkDevice vkDevice = device->GetHandle();

    VkBufferCreateInfo info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkDevice, &info, nullptr, &outBuffer) != VK_SUCCESS)
        throw std::runtime_error("CreateBuffer: vkCreateBuffer failed");

    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(vkDevice, outBuffer, &memReq);

    VkMemoryAllocateInfo alloc{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc.allocationSize = memReq.size;
    alloc.memoryTypeIndex = device->FindMemoryType(memReq.memoryTypeBits, props);

    if (vkAllocateMemory(vkDevice, &alloc, nullptr, &outMemory) != VK_SUCCESS)
        throw std::runtime_error("CreateBuffer: vkAllocateMemory failed");

    vkBindBufferMemory(vkDevice, outBuffer, outMemory, 0);
}

static void TransitionImageLayout(
    VulkanDevice* device,
    VulkanCommandPool* cmdPool,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout)
{
    VkCommandBuffer cmd = cmdPool->BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage = 0;
    VkPipelineStageFlags dstStage = 0;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::runtime_error("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(
        cmd,
        srcStage, dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    cmdPool->EndSingleTimeCommands(cmd);
}

static void CopyBufferToImage(
    VulkanCommandPool* cmdPool,
    VkBuffer buffer,
    VkImage image,
    uint32_t w,
    uint32_t h)
{
    VkCommandBuffer cmd = cmdPool->BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0,0,0 };
    region.imageExtent = { w, h, 1 };

    vkCmdCopyBufferToImage(
        cmd,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    cmdPool->EndSingleTimeCommands(cmd);
}

VulkanTexture2D::VulkanTexture2D(
    VulkanDevice* device,
    VulkanCommandPool* cmdPool,
    const void* rgbaPixels,
    uint32_t width,
    uint32_t height)
    : m_Device(device), m_CmdPool(cmdPool)
{
    CreateImage(width, height);
    Upload(rgbaPixels, width, height);
    CreateView();
    CreateSampler();
    LOG_INFO("VulkanTexture2D created.");
}

VulkanTexture2D::~VulkanTexture2D()
{
    VkDevice vkDevice = m_Device->GetHandle();
    if (m_Sampler) vkDestroySampler(vkDevice, m_Sampler, nullptr);
    if (m_View)    vkDestroyImageView(vkDevice, m_View, nullptr);
    if (m_Image)   vkDestroyImage(vkDevice, m_Image, nullptr);
    if (m_Memory)  vkFreeMemory(vkDevice, m_Memory, nullptr);
}

void VulkanTexture2D::CreateImage(uint32_t w, uint32_t h)
{
    VkDevice vkDevice = m_Device->GetHandle();

    VkImageCreateInfo info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    info.imageType = VK_IMAGE_TYPE_2D;
    info.extent = { w, h, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.format = m_Format;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(vkDevice, &info, nullptr, &m_Image) != VK_SUCCESS)
        throw std::runtime_error("CreateImage: vkCreateImage failed");

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(vkDevice, m_Image, &memReq);

    VkMemoryAllocateInfo alloc{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    alloc.allocationSize = memReq.size;
    alloc.memoryTypeIndex =
        m_Device->FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(vkDevice, &alloc, nullptr, &m_Memory) != VK_SUCCESS)
        throw std::runtime_error("CreateImage: vkAllocateMemory failed");

    vkBindImageMemory(vkDevice, m_Image, m_Memory, 0);
}

void VulkanTexture2D::Upload(const void* rgbaPixels, uint32_t w, uint32_t h)
{
    VkDeviceSize size = VkDeviceSize(w) * VkDeviceSize(h) * 4;

    VkBuffer staging = VK_NULL_HANDLE;
    VkDeviceMemory stagingMem = VK_NULL_HANDLE;

    CreateBuffer(
        m_Device,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging,
        stagingMem
    );

    void* mapped = nullptr;
    vkMapMemory(m_Device->GetHandle(), stagingMem, 0, size, 0, &mapped);
    std::memcpy(mapped, rgbaPixels, (size_t)size);
    vkUnmapMemory(m_Device->GetHandle(), stagingMem);

    TransitionImageLayout(
        m_Device, m_CmdPool, m_Image, m_Format,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(m_CmdPool, staging, m_Image, w, h);

    TransitionImageLayout(
        m_Device, m_CmdPool, m_Image, m_Format,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(m_Device->GetHandle(), staging, nullptr);
    vkFreeMemory(m_Device->GetHandle(), stagingMem, nullptr);
}

void VulkanTexture2D::CreateView()
{
    VkDevice vkDevice = m_Device->GetHandle();

    VkImageViewCreateInfo info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    info.image = m_Image;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = m_Format;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vkDevice, &info, nullptr, &m_View) != VK_SUCCESS)
        throw std::runtime_error("CreateView: vkCreateImageView failed");
}

void VulkanTexture2D::CreateSampler()
{
    VkDevice vkDevice = m_Device->GetHandle();

    VkSamplerCreateInfo info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.maxAnisotropy = 1.0f;
    info.anisotropyEnable = VK_FALSE;
    info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;

    if (vkCreateSampler(vkDevice, &info, nullptr, &m_Sampler) != VK_SUCCESS)
        throw std::runtime_error("CreateSampler: vkCreateSampler failed");
}
