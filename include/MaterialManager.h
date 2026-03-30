#pragma once
#include "Material.h"
#include "ResourceManager.h"

#include <fstream>

class MaterialManager : public ResourceManager<MaterialManager, Material>
{
public:
    MaterialManager(SharedContext* l_sharedContext, const fs::path& l_materialDirectory)
        : ResourceManager(l_sharedContext, l_materialDirectory) {}

    std::shared_ptr<Material> Load(const fs::path& l_path)
    {
        return std::make_shared<Material>(l_path, m_sharedContext);
    }
};