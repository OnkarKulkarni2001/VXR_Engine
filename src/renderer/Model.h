#pragma once
#include "Mesh.h"
#include "Transform.h"

class Model
{
public:
    Model(Mesh* mesh);

    void Draw(VkCommandBuffer cmd) const;

    Transform transform;

private:
    Mesh* m_Mesh = nullptr;
};
