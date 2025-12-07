#include "Application.h"
#include "Logger.h"
#include "../renderer/VulkanInstance.h"

Application::Application() {
    m_Window = new Window(1280, 720, "VXR Engine");
    Logger::Log("Application Initialized");
}

void Application::Run() {
    VulkanInstance instance(true);  // enable validation layers

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
    }
}
