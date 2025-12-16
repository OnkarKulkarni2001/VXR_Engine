#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool ShouldClose() const;
    GLFWwindow* GetHandle() const { return m_Window; }

    // NEW — needed for Vulkan
    VkSurfaceKHR CreateSurface(VkInstance instance);
    VkSurfaceKHR GetSurface() const { return m_Surface; }

    int GetWidth()  const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window = nullptr;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

    int m_Width;
    int m_Height;
};
