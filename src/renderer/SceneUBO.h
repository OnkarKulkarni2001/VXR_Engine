#pragma once
#include <glm/glm.hpp>
#include "../lighting/SceneLighting.h"

struct SceneUBO
{
    glm::mat4 view;
    glm::mat4 projection;

    SceneLighting lighting;
};
