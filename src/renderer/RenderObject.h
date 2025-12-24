#pragma once

#include "Transform.h"

class Mesh;
class MaterialInstance;
class VulkanPipeline;

struct RenderObject
{
    Transform transform;
    Mesh* mesh = nullptr;

    MaterialInstance* material = nullptr;
	VulkanPipeline* pipeline = nullptr;
};
