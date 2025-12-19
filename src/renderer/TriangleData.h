#pragma once
#include <vector>
#include "Vertex.h"

inline const std::vector<Vertex> TRIANGLE_VERTICES = {
    {{ 0.0f,  0.5f }, {1,0,0}},
    {{ 0.5f, -0.5f }, {0,1,0}},
    {{-0.5f, -0.5f }, {0,0,1}},
};

// NEW
inline const std::vector<uint32_t> TRIANGLE_INDICES = {
    0, 1, 2
};
