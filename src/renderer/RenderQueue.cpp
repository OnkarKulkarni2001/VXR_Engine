#include "RenderQueue.h"
#include "RenderObject.h"

void RenderQueue::Clear()
{
    m_Commands.clear();
}

void RenderQueue::Submit(const RenderObject& obj)
{
    if (!obj.mesh || !obj.pipeline)
        return;

    RenderCommand cmd{};
    cmd.mesh = obj.mesh;
    cmd.pipeline = obj.pipeline;

    // IMPORTANT: use YOUR existing Transform logic
    cmd.model = obj.transform.ToMatrix();

    m_Commands.push_back(cmd);
}

const std::vector<RenderCommand>& RenderQueue::GetCommands() const
{
    return m_Commands;
}
