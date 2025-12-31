#pragma once
#include "LightingTypes.h"
#include "LightingLimits.h"

struct SceneLighting
{
    DirectionalLight sun;

    uint32_t pointLightCount;
    uint32_t spotLightCount;
    glm::vec2 padding; // std140

    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight  spotLights[MAX_SPOT_LIGHTS];
};
