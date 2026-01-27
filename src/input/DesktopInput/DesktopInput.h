#pragma once
#include <GLFW/glfw3.h>

class DesktopInput
{
public:
    explicit DesktopInput(GLFWwindow* window);

    void Update();

    float GetMouseDX() const;
    float GetMouseDY() const;
    float GetScroll()  const;

    bool IsKeyDown(int key) const;

private:
    GLFWwindow* m_Window;

    double m_LastX = 0.0;
    double m_LastY = 0.0;
    bool   m_FirstMouse = true;

    float  m_MouseDX = 0.0f;
    float  m_MouseDY = 0.0f;
    float  m_Scroll = 0.0f;
};
