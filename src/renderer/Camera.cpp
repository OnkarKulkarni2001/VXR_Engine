#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

glm::vec3 Camera::Forward() const
{
    // Yaw around Y, Pitch around X
    // Forward points toward -Z when yaw=0, pitch=0 (common convention)
    const float cy = cosf(m_Yaw);
    const float sy = sinf(m_Yaw);
    const float cp = cosf(m_Pitch);
    const float sp = sinf(m_Pitch);

    glm::vec3 f;
    f.x = sy * cp;
    f.y = sp;
    f.z = -cy * cp;

    return glm::normalize(f);
}

glm::vec3 Camera::Right() const
{
    return glm::normalize(glm::cross(Forward(), m_WorldUp));
}

void Camera::ClampPitch()
{
    // Avoid gimbal lock / flip. Keep pitch just below +/- 90 degrees.
    const float limit = glm::half_pi<float>() - 0.001f;
    m_Pitch = std::clamp(m_Pitch, -limit, limit);
}

glm::mat4 Camera::GetView() const
{
    const glm::vec3 f = Forward();
    return glm::lookAt(m_Position, m_Position + f, m_WorldUp);
}

glm::mat4 Camera::GetProjection(float aspect) const
{
    glm::mat4 proj = glm::perspective(m_Fov, aspect, m_Near, m_Far);

    // Vulkan clip-space correction (Y inverted)
    proj[1][1] *= -1.0f;

    return proj;
}

void Camera::MoveForward(float amount)
{
    m_Position += Forward() * amount;
}

void Camera::MoveRight(float amount)
{
    m_Position += Right() * amount;
}

void Camera::MoveUp(float amount)
{
    m_Position += m_WorldUp * amount;
}
