#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Camera
{
public:

    Camera();

    // Camera configuration
    void SetPosition(const glm::vec3& p) { m_Position = p; }
    const glm::vec3& GetPosition() const { return m_Position; }

    // Yaw/Pitch in radians (engine-friendly)
    void SetYawPitch(float yawRadians, float pitchRadians)
    {
        m_Yaw = yawRadians;
        m_Pitch = pitchRadians;

    }

    void AddYawPitch(float yawDelta, float pitchDelta)
    {
        m_Yaw += yawDelta;
        m_Pitch += pitchDelta;



        UpdateBasis();
    }

    void SetPerspective(float fovRadians, float nearZ, float farZ)
    {
        m_Fov = fovRadians;
        m_Near = nearZ;
        m_Far = farZ;
    }

    // Matrices
    glm::mat4 GetView() const;
    glm::mat4 GetProjection(float aspect) const;

    // Movement helpers (local-space)
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveUp(float amount);

private:
    void UpdateBasis();


private:
    glm::vec3 m_Position{ 0.0f, 0.0f, 2.0f };

    // radians
    float m_Yaw = 0.0f;   // rotate around Y
    float m_Pitch = 0.0f;   // rotate around X

    // perspective
    float m_Fov = 1.0472f; // ~60 deg
    float m_Near = 0.1f;
    float m_Far = 100.0f;

    glm::vec3 m_Forward{ 0, 0, -1 };
    glm::vec3 m_Right{ 1, 0,  0 };
    glm::vec3 m_Up{ 0, 1,  0 };

    glm::vec3 m_WorldUp{ 0, 1, 0 };
};
