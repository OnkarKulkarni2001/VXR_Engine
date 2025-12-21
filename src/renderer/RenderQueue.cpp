#include "RenderQueue.h"
#include "RenderObject.h"
#include "MaterialInstance.h"   // REQUIRED

void RenderQueue::Clear()
{
    m_Commands.clear();
}

void RenderQueue::Submit(const RenderObject& obj)
{
    // Validate required data
    if (!obj.mesh || !obj.material)
        return;

    RenderCommand cmd{};
    cmd.mesh = obj.mesh;

    // Pipeline comes from MaterialTemplate via MaterialInstance
    cmd.pipeline = obj.material->GetPipeline();

    // Per-object transform
    cmd.model = obj.transform.ToMatrix();

    // IMPORTANT: set = 1 descriptor set (albedo + normal)
    cmd.materialSet = obj.material->GetDescriptorSet();

    m_Commands.push_back(cmd);
}

const std::vector<RenderCommand>& RenderQueue::GetCommands() const
{
    return m_Commands;
}
