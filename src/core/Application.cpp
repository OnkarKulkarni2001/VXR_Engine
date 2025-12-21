#include "Application.h"
#include "Logger.h"

#include "renderer/VulkanInstance.h"
#include "renderer/VulkanDevice.h"
#include "renderer/VulkanSwapchain.h"
#include "renderer/VulkanDepthBuffer.h"
#include "renderer/VulkanMSAAColorBuffer.h"
#include "renderer/VulkanRenderPass.h"
#include "renderer/VulkanFramebuffers.h"
#include "renderer/VulkanCommandPool.h"
#include "renderer/VulkanCommandBuffers.h"
#include "renderer/VulkanSync.h"
#include "renderer/pipeline/VulkanPipeline.h"
#include "renderer/VulkanVertexBuffer.h"
#include "renderer/Vertex.h"
#include "renderer/TriangleData.h"
#include "renderer/UniformBufferObject.h"
#include "renderer/VulkanUniformBuffers.h"
#include "renderer/VulkanDescriptors.h"
#include "renderer/CameraUBO.h"
#include "renderer/VulkanIndexBuffer.h"
#include "renderer/Mesh.h"
#include "renderer/PushConstants.h"
#include "renderer/RenderObject.h"
#include "renderer/Scene.h"
#include "renderer/RenderQueue.h"
#include "renderer/Camera.h"
#include "input/CameraController.h"
#include "asset/ModelLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <array>

Application::Application()
{
    m_Window = new Window(1280, 720, "VXR Engine");
    Logger::Log("Application Initialized");
}

Application::~Application()
{
    Shutdown();
    //// If device exists, ensure GPU is idle before destroying GPU resources
    //if (m_Device)
    //    vkDeviceWaitIdle(m_Device->GetHandle());

    //// Destroy in reverse order of creation
    //delete m_Sync;
    //delete m_TriangleMesh;
    //delete m_CameraController;
    //delete m_Camera;

    //delete m_CommandBuffers;
    //delete m_IndexBuffer;
    //delete m_CommandPool;

    //delete m_Descriptors;
    //delete m_UniformBuffers;

    //delete m_Pipeline;
    //delete m_Framebuffers;
    //delete m_RenderPass;
    //delete m_MSAAColor;
    //delete m_DepthBuffer;
    //delete m_Swapchain;

    //delete m_VertexBuffer;

    //delete m_Device;

    //// IMPORTANT: Destroy surface BEFORE instance
    //if (m_Instance && m_Surface != VK_NULL_HANDLE)
    //{
    //    vkDestroySurfaceKHR(m_Instance->GetHandle(), m_Surface, nullptr);
    //    m_Surface = VK_NULL_HANDLE;
    //}

    //delete m_Instance;
    //delete m_Window;

    //m_Sync = nullptr;
    //m_CommandBuffers = nullptr;
    //m_IndexBuffer = nullptr;
    //m_CommandPool = nullptr;
    //m_Pipeline = nullptr;
    //m_Framebuffers = nullptr;
    //m_RenderPass = nullptr;
    //m_MSAAColor = nullptr;
    //m_DepthBuffer = nullptr;
    //m_Swapchain = nullptr;
    //m_VertexBuffer = nullptr;
    //m_Device = nullptr;
    //m_Instance = nullptr;
    //m_Window = nullptr;
    //m_TriangleMesh = nullptr;
    //m_Camera = nullptr;
    //m_CameraController = nullptr;

}

void Application::Shutdown()
{
    if (!m_Device)
        return;

    LOG_INFO("Application Shutdown started");

    // 1️⃣ Ensure GPU is idle
    vkDeviceWaitIdle(m_Device->GetHandle());

    // 2️⃣ Destroy scene + mesh ownership FIRST
    m_Scene.Clear();          // if implemented
    m_RenderObjects.clear();  // safe
    m_OwnedMeshes.clear();    // 🔥 MOST IMPORTANT FIX

    // Old demo mesh path (temporary, keep until removed)
    delete m_TriangleMesh;
    m_TriangleMesh = nullptr;

    delete m_VertexBuffer;
    delete m_IndexBuffer;
    m_VertexBuffer = nullptr;
    m_IndexBuffer = nullptr;

    // 3️⃣ Destroy per-frame + draw infrastructure
    delete m_Sync;
    delete m_CommandBuffers;
    delete m_CommandPool;

    m_Sync = nullptr;
    m_CommandBuffers = nullptr;
    m_CommandPool = nullptr;

    // 4️⃣ Descriptor + uniform systems
    delete m_Descriptors;
    delete m_UniformBuffers;
    m_Descriptors = nullptr;
    m_UniformBuffers = nullptr;

    // 5️⃣ Pipeline + render targets
    delete m_Pipeline;
    delete m_Framebuffers;
    delete m_RenderPass;
    delete m_MSAAColor;
    delete m_DepthBuffer;
    delete m_Swapchain;

    m_Pipeline = nullptr;
    m_Framebuffers = nullptr;
    m_RenderPass = nullptr;
    m_MSAAColor = nullptr;
    m_DepthBuffer = nullptr;
    m_Swapchain = nullptr;

    // 6️⃣ Camera & input
    delete m_CameraController;
    delete m_Camera;
    m_CameraController = nullptr;
    m_Camera = nullptr;

    // 7️⃣ Destroy device LAST
    delete m_Device;
    m_Device = nullptr;

    // 8️⃣ Surface before instance
    if (m_Instance && m_Surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_Instance->GetHandle(), m_Surface, nullptr);
        m_Surface = VK_NULL_HANDLE;
    }

    delete m_Instance;
    m_Instance = nullptr;

    delete m_Window;
    m_Window = nullptr;

    LOG_INFO("Application Shutdown completed");
}


void Application::Run()
{
    // 1) Instance
    m_Instance = new VulkanInstance(true);

    // 2) Surface + Device
    // Store surface in a member so we can destroy it later correctly.
    m_Surface = m_Window->CreateSurface(m_Instance->GetHandle());

    m_Device = new VulkanDevice(
        m_Instance->GetHandle(),
        m_Surface
    );

	// Uniform buffers + descriptors
    const uint32_t FRAMES_IN_FLIGHT = 2;

    m_UniformBuffers = new VulkanUniformBuffers(
        m_Device,
        FRAMES_IN_FLIGHT,
        sizeof(CameraUBO)
    );

    m_Descriptors = new VulkanDescriptors(
        m_Device,
        m_UniformBuffers,
        FRAMES_IN_FLIGHT
    );


    // 3) Swapchain
    m_Swapchain = new VulkanSwapchain(
        m_Instance->GetHandle(),
        m_Device,
        m_Surface,
        m_Window->GetWidth(),
        m_Window->GetHeight()
    );

    // Swapchain-derived formats
    VkExtent2D extent = m_Swapchain->GetExtent();
    VkFormat   colorFmt = m_Swapchain->GetImageFormat();
    VkFormat   depthFmt = m_Device->FindDepthFormat();

    // 4) Depth (MSAA aware)
    m_DepthBuffer = new VulkanDepthBuffer(m_Device, extent);

    // 5) MSAA Color
    m_MSAAColor = new VulkanMSAAColorBuffer(m_Device, colorFmt, extent);

    // 6) RenderPass
    m_RenderPass = new VulkanRenderPass(m_Device, colorFmt, depthFmt);

    // 7) Framebuffers
    m_Framebuffers = new VulkanFramebuffers(
        m_Device,
        m_Swapchain,
        m_RenderPass,
        m_DepthBuffer,
        m_MSAAColor
    );

	// Pipeline set layouts
    std::vector<VkDescriptorSetLayout> pipelineSetLayouts = {
        m_Descriptors->GetLayout() // set = 0
    };

    // 8) Pipeline (triangle shaders)
    m_Pipeline = new VulkanPipeline(
        m_Device,
        m_Swapchain,
        m_RenderPass,
		pipelineSetLayouts,
        "shaders/triangle.vert.spv",
        "shaders/triangle.frag.spv"
    );


    // Disable cursor for camera movement
    GLFWwindow* wnd = m_Window->GetHandle();
    //glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Camera setup
    m_Camera = new Camera();
    m_Camera->SetPosition({ 0.0f, 0.0f, 2.0f });
    m_Camera->SetYawPitch(0.0f, 0.0f);
    m_Camera->SetPerspective(glm::radians(60.0f), 0.1f, 100.0f);

    m_LastTime = (float)glfwGetTime();

	// Camera controller 
    m_CameraController = new CameraController(m_Camera);


	//  Mesh creation
    //m_TriangleMesh = new Mesh(
    //    m_Device,
    //    TRIANGLE_VERTICES.data(),
    //    VkDeviceSize(TRIANGLE_VERTICES.size() * sizeof(TRIANGLE_VERTICES[0])),
    //    sizeof(TRIANGLE_VERTICES[0]),
    //    TRIANGLE_INDICES.data(),
    //    VkDeviceSize(TRIANGLE_INDICES.size() * sizeof(TRIANGLE_INDICES[0])),
    //    (uint32_t)TRIANGLE_INDICES.size()
    //);
    auto meshes = ModelLoader::LoadStaticModel(
        m_Device,
        "G:/VXR_Engine/assets/monkey.glb" // or .fbx / .obj
    );

    for (auto& mesh : meshes)
    {
        RenderObject& obj = m_Scene.CreateObject();
        obj.mesh = mesh.get();
		obj.material = nullptr; // TODO: assign material
        obj.transform.position = { 0.0f, 0.0f, 0.0f };

        m_OwnedMeshes.push_back(std::move(mesh));
    }


    // 10) Command Pool + Command Buffers
    m_CommandPool = new VulkanCommandPool(m_Device);

    m_CommandBuffers = new VulkanCommandBuffers(
        m_Device,
        m_CommandPool,
        m_Swapchain,
        m_RenderPass,
        m_Framebuffers
    );

    // 11) Sync
    m_Sync = new VulkanSync(
        m_Device,
        2, // maxFramesInFlight
        static_cast<uint32_t>(m_Swapchain->GetImageViews().size())
    );

    // Main loop
    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(m_Device->GetHandle());
}

void Application::DrawFrame()
{
    // 1) CPU-GPU pacing: wait for this frame slot
    VkFence& inFlightFence = m_Sync->GetInFlightFence();

    vkWaitForFences(
        m_Device->GetHandle(),
        1,
        &inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    vkResetFences(m_Device->GetHandle(), 1, &inFlightFence);

    // 2) Acquire swapchain image
    uint32_t imageIndex = 0;
    VkSemaphore& imageAvailable = m_Sync->GetImageAvailableSemaphore(); // per-frame

    VkResult acquireResult = vkAcquireNextImageKHR(
        m_Device->GetHandle(),
        m_Swapchain->GetHandle(),
        UINT64_MAX,
        imageAvailable,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        LOG_WARN("Swapchain out of date (resize). Skipping frame.");
        return;
    }
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR("Failed to acquire swapchain image!");
        return;
    }


    // --- Delta time
    float now = (float)glfwGetTime();
    float dt = now - m_LastTime;
    m_LastTime = now;

    m_CameraController->Update(m_Window->GetHandle(), dt);

    // --- Update uniform buffer (per-frame)
    uint32_t frame = m_Sync->GetCurrentFrame();

    CameraUBO ubo{};
    ubo.view = m_Camera->GetView();

    float aspect =
        (float)m_Swapchain->GetExtent().width /
        (float)m_Swapchain->GetExtent().height;

    ubo.proj = m_Camera->GetProjection(aspect);

    m_UniformBuffers->Update(frame, &ubo, sizeof(ubo));



    // 3) Record this image's command buffer
    VkCommandBuffer cmd = m_CommandBuffers->GetBuffers()[imageIndex];

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to begin command buffer!");
        return;
    }

    // Render pass begin
    VkExtent2D extent = m_Swapchain->GetExtent();

    std::array<VkClearValue, 2> clears{};
    clears[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };
    clears[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = m_RenderPass->GetHandle();
    rpBegin.framebuffer = m_Framebuffers->GetFramebuffers()[imageIndex];
    rpBegin.renderArea.offset = { 0, 0 };
    rpBegin.renderArea.extent = extent;
    rpBegin.clearValueCount = static_cast<uint32_t>(clears.size());
    rpBegin.pClearValues = clears.data();

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);


	// draw meshes
    RenderQueue renderQueue;
    renderQueue.Clear();

    for (const RenderObject& obj : m_Scene.GetObjects())
    {
        renderQueue.Submit(obj);
    }

    for (const RenderCommand& rc : renderQueue.GetCommands())
    {
        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            rc.pipeline->GetHandle()
        );

        // Set 0 (global/per-frame)
        VkDescriptorSet globalSet0 = m_Descriptors->GetSet(frame);

        // Set 1 (material) 
        VkDescriptorSet materialSet1 = rc.materialSet; 

        VkDescriptorSet sets[] = { globalSet0, materialSet1 };

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            rc.pipeline->GetLayout(),   // IMPORTANT: match the currently bound pipeline layout
            0,
            2,
            sets,
            0,
            nullptr
        );


        PushConstants pc{};
        pc.model = rc.model; 

        vkCmdPushConstants(
            cmd,
            rc.pipeline->GetLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PushConstants),
            &pc
        );

        rc.mesh->Draw(cmd);
    }




    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to record command buffer!");
        return;
    }

    // 4) Submit
    VkSemaphore waitSemaphores[] = { imageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSemaphore& renderFinished = m_Sync->GetRenderFinishedSemaphore(imageIndex); // per-image
    VkSemaphore signalSemaphores[] = { renderFinished };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(
        m_Device->GetGraphicsQueue(),
        1,
        &submitInfo,
        inFlightFence
    ) != VK_SUCCESS)
    {
        LOG_ERROR("Failed to submit draw command buffer!");
        return;
    }

    // 5) Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { m_Swapchain->GetHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult presentResult = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
        LOG_WARN("Swapchain out of date/suboptimal on present. Skipping.");
    }
    else if (presentResult != VK_SUCCESS)
    {
        LOG_ERROR("Failed to present swapchain image!");
    }

    // 6) Advance frame slot
    m_Sync->AdvanceFrame();
}
