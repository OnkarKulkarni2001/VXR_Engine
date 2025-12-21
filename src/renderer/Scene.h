#pragma once
#include <vector>
#include "RenderObject.h"

class Scene
{
public:
    RenderObject& CreateObject();
    const std::vector<RenderObject>& GetObjects() const;

private:
    std::vector<RenderObject> m_Objects;
};
