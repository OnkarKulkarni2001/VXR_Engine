#include "Model.h"

Model::Model(Mesh* mesh)
    : m_Mesh(mesh)
{
}

void Model::Draw(VkCommandBuffer cmd) const
{
    m_Mesh->Draw(cmd);
}
