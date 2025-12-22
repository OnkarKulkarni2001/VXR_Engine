#pragma once

#include "renderer/Scene.h"
#include "renderer/MaterialTemplate.h"
#include "renderer/VulkanMaterialDescriptors.h"

#include "Window.h"
#include <vector>
#include <memory>

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanDepthBuffer;
class VulkanMSAAColorBuffer;
class VulkanRenderPass;
class VulkanFramebuffers;
class VulkanCommandPool;
class VulkanCommandBuffers;
class VulkanSync;
class VulkanPipeline;
class VulkanVertexBuffer;
class VulkanUniformBuffers;
class VulkanDescriptors;
class VulkanIndexBuffer;
class Mesh;
class RenderObject;
class Camera;
class CameraController;
class MaterialInstance;

class Application {
public:
    Application();
    ~Application();

    void Shutdown();

    void Run();

    void DrawFrame();
    
public:
    std::vector<RenderObject> m_RenderObjects;
    std::vector<std::unique_ptr<Mesh>> m_OwnedMeshes;
    Scene m_Scene;

private:
    Window* m_Window = nullptr;

    VulkanInstance* m_Instance = nullptr;
    VulkanDevice* m_Device = nullptr;
    VulkanSwapchain* m_Swapchain = nullptr;
    VulkanDepthBuffer* m_DepthBuffer = nullptr;
    VulkanMSAAColorBuffer* m_MSAAColor = nullptr;
    VulkanRenderPass* m_RenderPass = nullptr;
    VulkanFramebuffers* m_Framebuffers = nullptr;
    VulkanCommandPool* m_CommandPool = nullptr;
    VulkanCommandBuffers* m_CommandBuffers = nullptr;
    VulkanSync* m_Sync = nullptr;
    VkSurfaceKHR          m_Surface = VK_NULL_HANDLE;
    VulkanPipeline* m_Pipeline = nullptr;
    VulkanVertexBuffer* m_VertexBuffer = nullptr;
    VulkanUniformBuffers* m_UniformBuffers = nullptr;
    VulkanDescriptors* m_Descriptors = nullptr;
    VulkanIndexBuffer* m_IndexBuffer = nullptr;
    
    Mesh* m_TriangleMesh = nullptr;

    Camera* m_Camera = nullptr;
    CameraController* m_CameraController = nullptr;

	MaterialTemplate* m_MaterialTemplate = nullptr;
    MaterialInstance* m_DefaultMaterial = nullptr;
	VulkanMaterialDescriptors* m_MaterialPool = nullptr;

    float   m_LastTime = 0.0f;

	// Mouse input handling
    bool   m_FirstMouse = true;
    double m_LastMouseX = 0.0;
    double m_LastMouseY = 0.0;

    float  m_MouseSensitivity = 0.0025f; // radians per pixel (tweak later)

};
