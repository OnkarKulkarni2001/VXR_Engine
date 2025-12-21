#include "Scene.h"

RenderObject& Scene::CreateObject()
{
    m_Objects.emplace_back();
    return m_Objects.back();
}

const std::vector<RenderObject>& Scene::GetObjects() const
{
    return m_Objects;
}
