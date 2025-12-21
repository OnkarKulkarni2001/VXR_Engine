#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>

struct Vertex3D
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription b{};
        b.binding = 0;
        b.stride = sizeof(Vertex3D);
        b.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return b;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> a{};

        a[0].binding = 0; a[0].location = 0;
        a[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        a[0].offset = offsetof(Vertex3D, position);

        a[1].binding = 0; a[1].location = 1;
        a[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        a[1].offset = offsetof(Vertex3D, normal);

        a[2].binding = 0; a[2].location = 2;
        a[2].format = VK_FORMAT_R32G32_SFLOAT;
        a[2].offset = offsetof(Vertex3D, uv);

        return a;
    }
};

