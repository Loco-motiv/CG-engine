#pragma once
#include "Mesh.h"
#include "ResourceManager.h"

class MeshManager : public ResourceManager<MeshManager, Mesh>
{
public:
    MeshManager(SharedContext* l_sharedContext, const fs::path& l_meshDirectory)
        : ResourceManager(l_sharedContext, l_meshDirectory) {}

    void InitPreload()
    {
        m_preLoadedResources["Quad"] = std::make_shared<Mesh>("Quad", m_sharedContext->graphics->MakeQuadMesh(), m_sharedContext);

        for (const auto& file : m_paths)
        {
            auto meshesData = m_sharedContext->graphics->MakeMeshes(file.second.string(), "resources/materials/");

            for (auto& [name, rawData] : meshesData)
            {
                std::string resourceName           = file.first + ":" + name;
                m_preLoadedResources[resourceName] = std::make_shared<Mesh>(resourceName, std::move(rawData), m_sharedContext);
                m_names.push_back(resourceName);
            }
        }
        // for (const auto& name : m_names)
        // {
        //     std::cout << name << '\n';
        // }
    }

    std::shared_ptr<Mesh> Load(const fs::path& l_path)
    {
        return nullptr;
    }
};