#pragma once
#include "LightingTypes.h"
#include <glm/glm.hpp>

inline DirectionalLight MakeSunLight()
{
    DirectionalLight l{};
    l.direction = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));
    l.color = glm::vec3(1.0f);
    l.intensity = 1.0f;
    return l;
}

inline PointLight MakePointLight(glm::vec3 pos, glm::vec3 color, float radius, float intensity)
{
    PointLight l{};
    l.position = pos;
    l.color = color;
    l.radius = radius;
    l.intensity = intensity;
    return l;
}

inline SpotLight MakeSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color,
    float inner, float outer, float range)
{
    SpotLight l{};
    l.position = pos;
    l.direction = glm::normalize(dir);
    l.color = color;
    l.innerCutoff = inner;
    l.outerCutoff = outer;
    l.range = range;
    return l;
}
