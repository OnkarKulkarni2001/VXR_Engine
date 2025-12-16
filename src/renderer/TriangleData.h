#pragma once
#include "Vertex.h"
#include <array>

const std::array<Vertex, 3> TRIANGLE_VERTICES = {
    Vertex{ {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    Vertex{ {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
    Vertex{ { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
};
