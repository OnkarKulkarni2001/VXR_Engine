#pragma once
#include <vulkan/vulkan.h>
#include "../core/Logger.h"

#define VK_CHECK(x)                                                     \
    do                                                                  \
    {                                                                   \
        VkResult err = x;                                               \
        if (err != VK_SUCCESS)                                          \
        {                                                               \
            LOG_ERROR("Vulkan error: %d at %s:%d", err, __FILE__, __LINE__); \
            __debugbreak();                                             \
        }                                                               \
    } while (0)
