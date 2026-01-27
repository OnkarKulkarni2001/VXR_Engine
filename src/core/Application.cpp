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

#include "xr/OpenXRInstance/OpenXRInstance.h"
#include "xr/XREyeViews/XREyeViews.h"
#include "xr/OpenXRSession/OpenXRSession.h"
#include "xr/OpenXRVulkanHelpers/OpenXRVulkanHelpers.h"

#include "camera/DesktopCameraController/DesktopCameraController.h"
#include "input/DesktopInput/DesktopInput.h"

#include "asset/ModelLoader.h"
#include "renderer/VulkanTexture2D.h"
#include "renderer/MaterialInstance.h"

#include "lighting/LightFactory.h"

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
    m_OwnedMeshes.clear();    

    // Destroy runtime materials first (they reference textures via descriptor sets)
    for (auto* m : m_RuntimeMaterials) delete m;
    m_RuntimeMaterials.clear();

    // Destroy runtime textures (destroys VkImage/VkView/VkSampler/VkMemory)
    for (auto* t : m_RuntimeTextures) delete t;
    m_RuntimeTextures.clear();

    delete m_DefaultMaterial;
    delete m_DefaultAlbedo;
    delete m_DefaultNormal;

    m_DefaultMaterial = nullptr;
    m_DefaultAlbedo = nullptr;
    m_DefaultNormal = nullptr;


    // 2. Materials (descriptor pools + layouts)
    delete m_MaterialTemplate;
    delete m_MaterialPool;
    m_MaterialTemplate = nullptr;
    m_MaterialPool = nullptr;

    // 3️ Destroy per-frame + draw infrastructure
    delete m_Sync;
    delete m_CommandBuffers;
    delete m_CommandPool;
    delete m_GraphicsCmdPool;
    delete m_TransferCmdPool;

    m_Sync = nullptr;
    m_CommandBuffers = nullptr;
    m_CommandPool = nullptr;
    m_GraphicsCmdPool = nullptr;
    m_TransferCmdPool = nullptr;

    // 4️ Descriptor + uniform systems
    delete m_Descriptors;
    delete m_UniformBuffers;
    m_Descriptors = nullptr;
    m_UniformBuffers = nullptr;

    // 5️ Pipeline + render targets
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

    // 6️ Camera & input
    delete m_Input;
    delete m_CameraController;
    delete m_Camera;
    m_CameraController = nullptr;
    m_Camera = nullptr;
    m_Input = nullptr;

	// OpenXR session
    if (m_XRSession)
    {
        delete m_XRSession;
        m_XRSession = nullptr;
    }

    delete m_XREyeViews;
    m_XREyeViews = nullptr;

    delete m_XRInstance;
    m_XRInstance = nullptr;

    // 7️ Destroy device LAST
    delete m_Device;
    m_Device = nullptr;

    // 8️ Surface before instance
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

    // --- 0) OpenXR FIRST ---
    m_XRInstance = new OpenXRInstance();
    m_XREyeViews = new XREyeViews();
    m_XREyeViews->Init(m_XRInstance->Get(), m_XRInstance->GetSystemId());

    // --- 1) Ask OpenXR what Vulkan instance/device extensions it requires ---
    std::vector<const char*> xrInstanceExts;
    std::vector<const char*> xrDeviceExts;

    GetOpenXRVulkanExtensions(m_XRInstance->Get(), m_XRInstance->GetSystemId(), xrInstanceExts, xrDeviceExts);

    // --- 2) Create Vulkan instance WITH OpenXR-required extensions ---
    m_Instance = new VulkanInstance(true, xrInstanceExts);   // <-- you add this overload

    // 2) Surface + Device
    // Store surface in a member so we can destroy it later correctly.
    m_Surface = m_Window->CreateSurface(m_Instance->GetHandle());

    // --- 4) Ask OpenXR which VkPhysicalDevice MUST be used ---
    VkPhysicalDevice xrPhysicalDevice = GetOpenXRVulkanGraphicsDevice(
        m_XRInstance->Get(),
        m_XRInstance->GetSystemId(),
        m_Instance->GetHandle()
    );

    if (xrPhysicalDevice == VK_NULL_HANDLE)
    {
        LOG_ERROR("OpenXR did not return a valid VkPhysicalDevice.");
        return;
    }

    //m_Device = new VulkanDevice(
    //    m_Instance->GetHandle(),
    //    m_Surface
    //);

    m_Device = new VulkanDevice(
        m_Instance->GetHandle(),
        xrPhysicalDevice,
        m_Surface,
        xrDeviceExts
	);

    // --- 6) Now create OpenXR session (binding to the Vulkan device you just created) ---
    m_XRSession = new OpenXRSession();
    m_XRSession->Create(
        m_XRInstance->Get(),
        m_XRInstance->GetSystemId(),
        m_Instance->GetHandle(),
        xrPhysicalDevice,
        m_Device->GetHandle(),
        m_Device->GetGraphicsQueueFamilyIndex(),
        0
    );

    auto xrViews = m_XREyeViews->GetViews();
    uint32_t w = xrViews[0].recommendedWidth;
    uint32_t h = xrViews[0].recommendedHeight;
    m_XRSession->CreateColorSwapchain(w, h);



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


    // 10) Command Pool + Command Buffers
    m_GraphicsCmdPool = new VulkanCommandPool(m_Device, CommandPoolType::Graphics);
    m_TransferCmdPool = new VulkanCommandPool(m_Device, CommandPoolType::Transfer);


    m_MaterialPool = new VulkanMaterialDescriptors(m_Device, 128);

    // Pipeline layouts (ONLY set = 0 for now)
    std::vector<VkDescriptorSetLayout> layouts = {
        m_Descriptors->GetLayout(), // set = 0 (Camera UBO)
        m_MaterialPool->GetLayout()  // set = 1 (Material textures)
    };

    m_MaterialTemplate = new MaterialTemplate(
        m_Device,
        m_Swapchain,
        m_RenderPass,
        layouts,
        "shaders/unlit3d.vert.spv",
        "shaders/unlit3d.frag.spv"
    );


    // Default textures (1x1)
    const uint8_t white[4] = { 255,255,255,255 };
    const uint8_t flatN[4] = { 128,128,255,255 };

    m_DefaultAlbedo = new VulkanTexture2D(m_Device, m_GraphicsCmdPool, white, 1, 1);
    m_DefaultNormal = new VulkanTexture2D(m_Device, m_GraphicsCmdPool, flatN, 1, 1);

    m_DefaultMaterial = new MaterialInstance(
        m_Device,
        m_MaterialPool,
        m_MaterialTemplate,
        m_DefaultAlbedo->GetView(), m_DefaultAlbedo->GetSampler(),
        m_DefaultNormal->GetView(), m_DefaultNormal->GetSampler()
    );




    // Create pipeline (use unlit3d shaders, set0 only)
    m_Pipeline = new VulkanPipeline(
        m_Device,
        m_Swapchain,
        m_RenderPass,
        layouts,
        //m_Descriptors->GetLayout(),
        "shaders/lighting.vert.spv",
        "shaders/lighting.frag.spv"
    );


    // Disable cursor for camera movement
    GLFWwindow* wnd = m_Window->GetHandle();


    //glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Camera setup
    m_Camera = new Camera();
    m_Camera->SetPosition({ 0.0f, 0.0f, 2.0f });
    m_Camera->SetYawPitch(0.0f, 0.0f);
    m_Camera->SetPerspective(glm::radians(60.0f), 0.1f, 10000.0f);

    m_LastTime = (float)glfwGetTime();

	// Camera controller 
    m_CameraController = new DesktopCameraController(*m_Camera);

    m_Input = new DesktopInput(wnd);

    glfwSetWindowUserPointer(wnd, m_CameraController);

    glfwSetScrollCallback(wnd,
        [](GLFWwindow* wnd, double, double yOffset)
        {
            auto* controller =
                static_cast<DesktopCameraController*>(glfwGetWindowUserPointer(wnd));
            controller->OnScroll(yOffset);
        });

	

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
    auto loadedMeshes = ModelLoader::LoadStaticModel(
        m_Device,
        //"C:/Users/onkar/Downloads/uploads_files_3053791_Matteuccia_Struthiopteris_FBX/Matteuccia_Struthiopteris_FBX/matteucia_struthiopteris_3.fbx"
        //"C:/Users/onkar/Downloads/6e48z1kc7r40-bugatti/bugatti/bugatti.obj"
        "G:/VXR_Engine/assets/selene.fbx"
         //"C:/Users/onkar/Downloads/uploads_files_6647155_01_Street_Wear_Mesh_and_Textures/Skeleton_Meshes/FBX/bodyShapeAof2/Street_Wear_Combined_Mesh_A.fbx"
         //"C:/Users/onkar/Downloads/uploads_files_642137_goku+Low+Poly(v1)(1)/goku real4armature.obj"
        //"C:/Users/onkar/Downloads/wolf/WOLF.OBJ"
    );

    for (auto& lm : loadedMeshes)
    {
        VulkanTexture2D* albedo =
            lm.albedoPath.empty()
            ? m_DefaultAlbedo
            : new VulkanTexture2D(m_Device, m_GraphicsCmdPool, lm.albedoPath );

        if (!lm.albedoPath.empty())
        {
            m_RuntimeTextures.push_back(albedo);
        }

        VulkanTexture2D* normal =
            lm.normalPath.empty()
            ? m_DefaultNormal
            : new VulkanTexture2D(m_Device, m_GraphicsCmdPool, lm.normalPath );

        if (!lm.normalPath.empty())
        {
            m_RuntimeTextures.push_back(normal);
        }


        MaterialInstance* mat = new MaterialInstance(
            m_Device,
            m_MaterialPool,
            m_MaterialTemplate,
            albedo->GetView(), albedo->GetSampler(),
            normal->GetView(), normal->GetSampler()
        );
		m_RuntimeMaterials.push_back(mat);

        RenderObject& obj = m_Scene.CreateObject();
        obj.mesh = lm.mesh.get();
        obj.material = mat;
        obj.pipeline = m_Pipeline;

        m_OwnedMeshes.push_back(std::move(lm.mesh));
    }


    m_CommandBuffers = new VulkanCommandBuffers(
        m_Device,
        m_GraphicsCmdPool,
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

	// Lighting UBO setup
    m_SceneUBO.lighting.sun = MakeSunLight();

    m_SceneUBO.lighting.pointLightCount = 1;
    m_SceneUBO.lighting.pointLights[0] =
        MakePointLight({ 0, 2, 0 }, { 1, 0.8f, 0.6f }, 10.0f, 2.0f);

    m_SceneUBO.lighting.spotLightCount = 0;

    // Main loop
    if (m_UseXR)
    {
        XRRun();
    }
    else
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }
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
    //m_LastTime = now;

    FrameContext frame;
    frame.deltaTime = now - m_LastTime;
    m_LastTime = now;

	uint32_t frameIndex = m_Sync->GetCurrentFrame();


    // 1) Update desktop input once per frame
    m_Input->Update();

    // 2) Mouse look only while RMB is held
    GLFWwindow* wnd = m_Window->GetHandle();
    const bool rmb = (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    // Capture cursor while looking (so mouse doesn't hit screen edges)
    glfwSetInputMode(wnd, GLFW_CURSOR, rmb ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (rmb)
    {
        m_CameraController->OnMouseDelta(m_Input->GetMouseDX(), m_Input->GetMouseDY());
    }

    // 3) Movement (WASDQE) - use dt
    const float speed = 5.0f; // or expose from controller
    if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) m_Camera->MoveForward(speed * dt);
    if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) m_Camera->MoveForward(-speed * dt);
    if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) m_Camera->MoveRight(speed * dt);
    if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) m_Camera->MoveRight(-speed * dt);
    if (glfwGetKey(wnd, GLFW_KEY_E) == GLFW_PRESS) m_Camera->MoveUp(speed * dt);
    if (glfwGetKey(wnd, GLFW_KEY_Q) == GLFW_PRESS) m_Camera->MoveUp(-speed * dt);


 //   CameraUBO ubo{};

 //   ubo.view = m_Camera->GetView();

 //   float aspect =
 //       (float)m_Swapchain->GetExtent().width /
 //       (float)m_Swapchain->GetExtent().height;

 //   ubo.proj = m_Camera->GetProjection(aspect);
	////ubo.proj[1][1] *= -1; // Vulkan clip correction

 //   m_UniformBuffers->Update(frame, &ubo, sizeof(ubo));

	// Update scene UBO (lighting info)
	m_SceneUBO.view = m_Camera->GetView();

    float aspect =
        (float)m_Swapchain->GetExtent().width /
		(float)m_Swapchain->GetExtent().height;

	m_SceneUBO.projection = m_Camera->GetProjection(aspect);

	m_UniformBuffers->Update(frameIndex, &m_SceneUBO, sizeof(m_SceneUBO));



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

    std::array<VkClearValue, 3> clears{};
    clears[0].color = { { 0.01f, 0.01f, 0.01f, 1.0f } }; // MSAA color attachment
    clears[1].depthStencil = { 1.0f, 0 };             // depth
    clears[2].color = { { 0.01f, 0.01f, 0.01f, 1.0f } }; // resolve attachment (often required)


    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass = m_RenderPass->GetHandle();
    rpBegin.framebuffer = m_Framebuffers->GetFramebuffers()[imageIndex];
    rpBegin.renderArea.offset = { 0, 0 };
    rpBegin.renderArea.extent = extent;
    rpBegin.clearValueCount = static_cast<uint32_t>(clears.size());
    rpBegin.pClearValues = clears.data();

    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(cmd, 0, 1, &scissor);


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

        //// Set 0 (global/per-frame)
        VkDescriptorSet globalSet0 = m_Descriptors->GetSet(frameIndex);
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

void Application::DrawXRFrame()
{
    XRFrameInfo frame{};
    if (!m_XRSession->BeginFrame(frame))
        return;

    if (!frame.shouldRender)
    {
        m_XRSession->EndFrame(frame, nullptr, 0);
        return;
    }

    // 1) Locate views (2 eyes)
    std::vector<XrView> locatedViews;
    m_XRSession->LocateViews(
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        frame.predictedDisplayTime,
        locatedViews
    );

    // 2) Acquire XR swapchain image
    uint32_t imageIndex = 0;
    if (!m_XRSession->AcquireSwapchainImage(imageIndex)) return;
    if (!m_XRSession->WaitSwapchainImage()) return;

    VkImage xrColorImage = m_XRSession->GetSwapchainImages()[imageIndex];

    // 3) Render into xrColorImage (YOU MUST IMPLEMENT THIS)
    //    This is where you record a command buffer that targets the XR swapchain VkImage.
    //    You cannot use m_Framebuffers (they are built for the window swapchain).
    RenderToXRSwapchainImage(xrColorImage, imageIndex, locatedViews);

    // 4) Release XR swapchain image
    m_XRSession->ReleaseSwapchainImage();

    // 5) Submit projection layer
    uint32_t w = m_XRSession->GetSwapchainWidth();
    uint32_t h = m_XRSession->GetSwapchainHeight();

    XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
    layer.space = m_XRSession->GetAppSpace();

    std::vector<XrCompositionLayerProjectionView> projViews(locatedViews.size());
    for (uint32_t i = 0; i < (uint32_t)locatedViews.size(); ++i)
    {
        projViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
        projViews[i].pose = locatedViews[i].pose;
        projViews[i].fov = locatedViews[i].fov;

        projViews[i].subImage.swapchain = m_XRSession->GetSwapchain();
        projViews[i].subImage.imageRect.offset = { 0, 0 };
        projViews[i].subImage.imageRect.extent = { (int32_t)w, (int32_t)h };
        projViews[i].subImage.imageArrayIndex = i; // layer 0/1
    }

    layer.viewCount = (uint32_t)projViews.size();
    layer.views = projViews.data();

    const XrCompositionLayerBaseHeader* layers[] =
    {
        (const XrCompositionLayerBaseHeader*)&layer
    };

    m_XRSession->EndFrame(frame, layers, 1);
}

void Application::RenderToXRSwapchainImage(VkImage xrColorImage, uint32_t /*imageIndex*/, const std::vector<XrView>& /*locatedViews*/)
{
    // Minimal: clear the XR swapchain image every frame.
    // This is intentionally simple: submit and wait (not optimal, but great for proving XR path).

    VkDevice device = m_Device->GetHandle();

    // Allocate a temporary command buffer from your graphics command pool
    VkCommandBufferAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandPool = m_GraphicsCmdPool->GetHandle();  // <-- Your VulkanCommandPool must expose GetHandle()
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = 1;

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(device, &alloc, &cmd) != VK_SUCCESS)
    {
        LOG_ERROR("XR: Failed to allocate command buffer for clear.");
        return;
    }

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin);

    // Transition XR image to GENERAL so we can clear it
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.oldLayout = m_XRImageLayoutInitialized ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_UNDEFINED;; // Safe for a first test; later track layouts properly
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = xrColorImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 2; // arraySize=2 swapchain (left+right)

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    // Clear color (pick anything obvious)
    VkClearColorValue clearColor{};
    clearColor.float32[0] = 0.02f;
    clearColor.float32[1] = 0.02f;
    clearColor.float32[2] = 0.08f;
    clearColor.float32[3] = 1.0f;

    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 2;

    vkCmdClearColorImage(cmd, xrColorImage, VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &range);

    // Barrier after clear (optional but keeps sync sane)
    VkImageMemoryBarrier barrier2 = barrier;
    barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier2.dstAccessMask = 0;
    barrier2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier2.newLayout = VK_IMAGE_LAYOUT_GENERAL;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier2
    );

    vkEndCommandBuffer(cmd);

    // Submit and wait (simple correctness first)
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence = VK_NULL_HANDLE;
    vkCreateFence(device, &fenceInfo, nullptr, &fence);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submit, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    
    m_XRImageLayoutInitialized = true;

    vkDestroyFence(device, fence, nullptr);
    vkFreeCommandBuffers(device, m_GraphicsCmdPool->GetHandle(), 1, &cmd);

}

void Application::XRRun()
{
    // XR loop owns polling + choosing XR vs desktop rendering
    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();

        // Drive OpenXR state machine
        m_XRSession->PollEvents();

        if (m_XRSession->IsRunning())
        {
            DrawXRFrame();
        }
        else
        {
            // Optional: render desktop while headset not active
            DrawFrame();
        }
    }
}
