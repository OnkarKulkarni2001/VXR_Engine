#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <cstdint>

class VulkanDevice;
class VulkanVertexBuffer;
class VulkanIndexBuffer;

class Mesh
{
public:
    Mesh(VulkanDevice* device,
        const void* vertexData, VkDeviceSize vertexBytes, uint32_t vertexStride,
        const void* indexData, VkDeviceSize indexBytes, uint32_t indexCount);

    ~Mesh();

    // records commands only (no submit/present)
    void Bind(VkCommandBuffer cmd) const;
    void Draw(VkCommandBuffer cmd) const;

    uint32_t GetIndexCount() const { return m_IndexCount; }

public:
    glm::mat4 Model = glm::mat4(1.0f);

private:
    VulkanDevice* m_Device = nullptr;

    VulkanVertexBuffer* m_VB = nullptr;
    VulkanIndexBuffer* m_IB = nullptr;

    uint32_t m_IndexCount = 0;
};
