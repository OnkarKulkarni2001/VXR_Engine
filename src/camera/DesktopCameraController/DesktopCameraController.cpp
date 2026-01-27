#include "DesktopCameraController.h"
#include <GLFW/glfw3.h>

DesktopCameraController::DesktopCameraController(Camera& camera)
    : m_Camera(camera)
{
}

void DesktopCameraController::Update(const FrameContext& frame)
{
    // movement handled via DesktopInput
}

void DesktopCameraController::OnMouseDelta(float dx, float dy)
{
    m_Camera.AddYawPitch(
        dx * m_MouseSensitivity,
        -dy * m_MouseSensitivity
    );
}

void DesktopCameraController::OnScroll(float offset)
{
    m_MoveSpeed *= (offset > 0.0f) ? 1.1f : 0.9f;
}
