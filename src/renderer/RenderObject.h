#pragma once

#include "Transform.h"

class Mesh;
class VulkanPipeline;

struct RenderObject
{
    Transform transform;
    Mesh* mesh = nullptr;
    VulkanPipeline* pipeline = nullptr;
};
