#include "Window.h"
#include "Logger.h"

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        Logger::Log("Failed to initialize GLFW", LogLevel::Error);
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_Window) {
        Logger::Log("Failed to create GLFW window", LogLevel::Error);
        glfwTerminate();
    }
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}
