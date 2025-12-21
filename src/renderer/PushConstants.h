#pragma once
#include <glm/glm.hpp>

struct PushConstants
{
    glm::mat4 model;   // 64 bytes
    //glm::vec4 tint;   // 16 bytes
    // total = 80 bytes (safe)
};
