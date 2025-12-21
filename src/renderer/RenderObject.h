#pragma once

#include "Transform.h"

class Mesh;
class MaterialInstance;

struct RenderObject
{
    Transform transform;
    Mesh* mesh = nullptr;

    MaterialInstance* material = nullptr;
};
