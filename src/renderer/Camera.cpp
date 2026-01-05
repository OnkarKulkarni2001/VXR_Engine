#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

Camera::Camera()
{
    UpdateBasis();
}


glm::mat4 Camera::GetView() const
{

    return glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

void Camera::UpdateBasis()
{
    const float cy = cosf(m_Yaw);
    const float sy = sinf(m_Yaw);
    const float cp = cosf(m_Pitch);
    const float sp = sinf(m_Pitch);

    m_Forward = glm::normalize(glm::vec3(
        sy * cp,
        sp,
        -cy * cp
    ));

    m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
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
    m_Position += m_Forward * amount;
}

void Camera::MoveRight(float amount)
{
    m_Position += m_Right * amount;
}

void Camera::MoveUp(float amount)
{
    m_Position += m_Up * amount;
}
