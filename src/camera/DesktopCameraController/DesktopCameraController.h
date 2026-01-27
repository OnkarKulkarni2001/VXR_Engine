#pragma once

#include "renderer/Camera.h"
#include "core/FrameContext.h"

class DesktopCameraController
{
public:
    explicit DesktopCameraController(Camera& camera);

    void Update(const FrameContext& frame);

    void OnMouseDelta(float dx, float dy);
    void OnScroll(float offset);

private:
    Camera& m_Camera;

    float m_MoveSpeed = 5.0f;
    float m_MouseSensitivity = 0.0025f;
};
