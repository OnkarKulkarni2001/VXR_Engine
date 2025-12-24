#include "RenderQueue.h"
#include "RenderObject.h"
#include "MaterialInstance.h"   // REQUIRED
#include "Mesh.h"               // REQUIRED

void RenderQueue::Clear()
{
    m_Commands.clear();
}

void RenderQueue::Submit(const RenderObject& obj)
{
    if (!obj.mesh || !obj.material)
        return;

    RenderCommand cmd{};
    cmd.mesh = obj.mesh;
    cmd.pipeline = obj.material->GetPipeline();
    cmd.materialSet = obj.material->GetDescriptorSet();
    cmd.model = obj.transform.ToMatrix();

    m_Commands.push_back(cmd);
}


const std::vector<RenderCommand>& RenderQueue::GetCommands() const
{
    return m_Commands;
}
