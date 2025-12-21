#pragma once
#include <GLFW/glfw3.h>

class Camera;

class CameraController
{
public:
    CameraController(Camera* camera);

    void Update(GLFWwindow* window, float dt);

    void SetMouseSensitivity(float s) { m_MouseSensitivity = s; }
    void SetMoveSpeed(float s) { m_MoveSpeed = s; }

private:
    Camera* m_Camera = nullptr;

    bool   m_FirstMouse = true;
    double m_LastX = 0.0;
    double m_LastY = 0.0;

    float  m_MouseSensitivity = 0.0025f;
    float  m_MoveSpeed = 2.0f;
};
