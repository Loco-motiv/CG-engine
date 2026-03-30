#pragma once
#include "ResourceManager.h"
#include "Shader.h"

class ShaderManager : public ResourceManager<ShaderManager, Shader>
{
public:
    ShaderManager(SharedContext* l_sharedContext, const fs::path& l_shaderDirectory)
        : ResourceManager(l_sharedContext, l_shaderDirectory) {}

    std::shared_ptr<Shader> Load(const fs::path& l_path)
    {
        return std::make_shared<Shader>(l_path, m_sharedContext);
    }
};