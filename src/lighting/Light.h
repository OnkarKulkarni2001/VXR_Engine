#pragma once
#include "LightingTypes.h"
#include <glm/glm.hpp>

inline DirectionalLight CreateDefaultSunLight()
{
    DirectionalLight light{};
    light.direction = glm::normalize(glm::vec3(-0.4f, -1.0f, -0.2f));
    light.color = glm::vec3(1.0f);
    light.intensity = 1.0f;
    return light;
}
