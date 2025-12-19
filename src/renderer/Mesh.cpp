#include "Mesh.h"
#include "VulkanDevice.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

Mesh::Mesh(VulkanDevice* device,
    const void* vertexData, VkDeviceSize vertexBytes, uint32_t /*vertexStride*/,
    const void* indexData, VkDeviceSize indexBytes, uint32_t indexCount)
    : m_Device(device), m_IndexCount(indexCount)
{
    // These should already use staging (as you just upgraded VB; do same for IB)
    m_VB = new VulkanVertexBuffer(m_Device, vertexData, vertexBytes);
    m_IB = new VulkanIndexBuffer(m_Device, indexData, indexBytes);
}

Mesh::~Mesh()
{
    delete m_IB;
    delete m_VB;
    m_IB = nullptr;
    m_VB = nullptr;
}

void Mesh::Bind(VkCommandBuffer cmd) const
{
    VkBuffer vb = m_VB->GetBuffer();
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, &vb, offsets);

    vkCmdBindIndexBuffer(cmd, m_IB->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::Draw(VkCommandBuffer cmd) const
{
    Bind(cmd);
    vkCmdDrawIndexed(cmd, m_IndexCount, 1, 0, 0, 0);
}
