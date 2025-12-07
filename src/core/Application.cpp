#include "Application.h"
#include "Logger.h"

Application::Application() {
    m_Window = new Window(1280, 720, "VXR Engine");
    Logger::Log("Application Initialized");
}

void Application::Run() {
    while (!m_Window->ShouldClose()) {
        glfwPollEvents();
    }
}
