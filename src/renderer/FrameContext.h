#pragma once
#include <vulkan/vulkan.h>

struct FrameContext
{
    uint32_t frameIndex;
    VkCommandBuffer cmd;
    VkDescriptorSet globalSet;
};
