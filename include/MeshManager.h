#pragma once
#include "Mesh.h"
#include "ResourceManager.h"

class MeshManager : public ResourceManager<MeshManager, Mesh>
{
public:
    MeshManager(SharedContext* l_sharedContext, const fs::path& l_meshDirectory)
        : ResourceManager(l_sharedContext, l_meshDirectory)
    {
        m_preLoadedResources["Quad"] = std::make_shared<Mesh>("Quad", m_sharedContext->graphics->MakeQuadMesh(), m_sharedContext);
    }

    std::shared_ptr<Mesh> Load(const fs::path& l_path)
    {
        return std::make_shared<Mesh>(l_path, m_sharedContext);
    }
};