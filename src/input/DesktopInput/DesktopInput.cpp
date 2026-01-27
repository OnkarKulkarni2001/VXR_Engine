#include "DesktopInput.h"

DesktopInput::DesktopInput(GLFWwindow* window)
    : m_Window(window)
{
}

void DesktopInput::Update()
{
    double x, y;
    glfwGetCursorPos(m_Window, &x, &y);

    if (m_FirstMouse)
    {
        m_LastX = x;
        m_LastY = y;
        m_FirstMouse = false;
    }

    m_MouseDX = static_cast<float>(x - m_LastX);
    m_MouseDY = static_cast<float>(y - m_LastY);

    m_LastX = x;
    m_LastY = y;

    m_Scroll = 0.0f; // set from callback later
}

float DesktopInput::GetMouseDX() const { return m_MouseDX; }
float DesktopInput::GetMouseDY() const { return m_MouseDY; }
float DesktopInput::GetScroll()  const { return m_Scroll; }

bool DesktopInput::IsKeyDown(int key) const
{
    return glfwGetKey(m_Window, key) == GLFW_PRESS;
}
