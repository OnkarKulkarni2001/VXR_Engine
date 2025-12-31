#pragma once
#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction;
    float intensity;

    glm::vec3 color;
    float padding;
};

struct PointLight
{
    glm::vec3 position;
    float radius;

    glm::vec3 color;
    float intensity;
};

struct SpotLight
{
    glm::vec3 position;
    float range;

    glm::vec3 direction;
    float innerCutoff;

    glm::vec3 color;
    float outerCutoff;
};
