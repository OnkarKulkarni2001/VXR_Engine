#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    GLFWwindow* GetNativeWindow() const { return m_Window; }
    bool ShouldClose() const;

private:
    GLFWwindow* m_Window = nullptr;
};
