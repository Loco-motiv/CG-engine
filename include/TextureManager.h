#pragma once
#include "ResourceManager.h"
#include "Texture.h"

class TextureManager : public ResourceManager<TextureManager, Texture>
{
public:
    TextureManager(SharedContext* l_sharedContext, const fs::path& l_textureDirectory)
        : ResourceManager(l_sharedContext, l_textureDirectory) {}

    std::shared_ptr<Texture> Load(const fs::path& l_path)
    {
        return std::make_shared<Texture>(l_path, m_sharedContext);
    }
};