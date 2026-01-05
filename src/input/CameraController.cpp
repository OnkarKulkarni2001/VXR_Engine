#include "CameraController.h"
#include "renderer/Camera.h"

//CameraController::CameraController(Camera* camera)
//    : m_Camera(camera)
//{
//}
//
//void CameraController::OnScroll(double yOffset)
//{
//    if (yOffset > 0)
//        m_MoveSpeed *= 1.15f;
//    else
//        m_MoveSpeed *= 0.85f;
//
//    m_MoveSpeed = glm::clamp(m_MoveSpeed, 0.05f, 100.0f);
//}
//
//void CameraController::Update(GLFWwindow* window, float dt)
//{
//    if (!m_Camera) return;
//
//    // ---- Keyboard movement (UNCHANGED)
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        m_Camera->MoveForward(m_MoveSpeed * dt);
//
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        m_Camera->MoveForward(-m_MoveSpeed * dt);
//
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        m_Camera->MoveRight(m_MoveSpeed * dt);
//
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        m_Camera->MoveRight(-m_MoveSpeed * dt);
//
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//        m_Camera->MoveUp(m_MoveSpeed * dt);
//
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//        m_Camera->MoveUp(-m_MoveSpeed * dt);
//
//    // ---- Mouse position
//    double x, y;
//    glfwGetCursorPos(window, &x, &y);
//
//    // ---- Mouse look ONLY when middle mouse is held
//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
//    {
//        if (m_FirstMouse)
//        {
//            m_LastX = x;
//            m_LastY = y;
//            m_FirstMouse = false;
//            return; // avoid jump on first press
//        }
//
//        double dx = x - m_LastX;
//        double dy = y - m_LastY;
//
//        m_LastX = x;
//        m_LastY = y;
//
//        m_Camera->AddYawPitch(
//            static_cast<float>(dx) * m_MouseSensitivity,
//            static_cast<float>(-dy) * m_MouseSensitivity
//        );
//    }
//    else
//    {
//        // Reset when mouse button released
//        m_FirstMouse = true;
//    }
//}

namespace
{
    // Hold Right Mouse Button to capture mouse + look around.
    constexpr int kLookMouseButton = GLFW_MOUSE_BUTTON_RIGHT;

    inline float SpeedMultiplier(GLFWwindow* window)
    {
        float mul = 1.0f;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            mul *= 3.0f;

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            mul *= 0.35f;

        return mul;
    }
}

CameraController::CameraController(Camera* camera)
    : m_Camera(camera)
{
}

void CameraController::OnScroll(double yOffset)
{
    if (yOffset > 0) m_MoveSpeed *= 1.15f;
    else            m_MoveSpeed *= 0.85f;

    m_MoveSpeed = glm::clamp(m_MoveSpeed, 0.05f, 100.0f);
}

void CameraController::Update(GLFWwindow* window, float dt)
{
    if (!m_Camera) return;

    const float speed = m_MoveSpeed * SpeedMultiplier(window);

    // ---- Keyboard movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_Camera->MoveForward(speed * dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_Camera->MoveForward(-speed * dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_Camera->MoveRight(speed * dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_Camera->MoveRight(-speed * dt);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) m_Camera->MoveUp(speed * dt);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) m_Camera->MoveUp(-speed * dt);

    // ---- Mouse position
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // ---- Mouse look when RMB is held (capture cursor so you can rotate forever)
    const bool looking = (glfwGetMouseButton(window, kLookMouseButton) == GLFW_PRESS);
    glfwSetInputMode(window, GLFW_CURSOR, looking ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (looking)
    {
        if (m_FirstMouse)
        {
            m_LastX = x;
            m_LastY = y;
            m_FirstMouse = false;
            return; // avoid jump on first press
        }

        const double dx = x - m_LastX;
        const double dy = y - m_LastY;

        m_LastX = x;
        m_LastY = y;

        m_Camera->AddYawPitch(
            static_cast<float>(dx) * m_MouseSensitivity,
            static_cast<float>(-dy) * m_MouseSensitivity
        );
    }
    else
    {
        m_FirstMouse = true;
    }
}