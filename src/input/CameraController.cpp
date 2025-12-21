#include "CameraController.h"
#include "renderer/Camera.h"

CameraController::CameraController(Camera* camera)
    : m_Camera(camera)
{
}

void CameraController::Update(GLFWwindow* window, float dt)
{
    if (!m_Camera) return;

    // ---- Keyboard movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera->MoveForward(m_MoveSpeed * dt);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera->MoveForward(-m_MoveSpeed * dt);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera->MoveRight(m_MoveSpeed * dt);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera->MoveRight(-m_MoveSpeed * dt);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_Camera->MoveUp(m_MoveSpeed * dt);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_Camera->MoveUp(-m_MoveSpeed * dt);

    // ---- Mouse look
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (m_FirstMouse)
    {
        m_LastX = x;
        m_LastY = y;
        m_FirstMouse = false;
    }

    double dx = x - m_LastX;
    double dy = y - m_LastY;

    m_LastX = x;
    m_LastY = y;

    m_Camera->AddYawPitch(
        static_cast<float>(dx) * m_MouseSensitivity,
        static_cast<float>(-dy) * m_MouseSensitivity
    );
}
