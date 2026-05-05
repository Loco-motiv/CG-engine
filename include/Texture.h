#pragma once
#include "ResourceManager.h"

class SharedContext;

class Texture
{
public:
    Texture(const fs::path& l_path, SharedContext* l_sharedContext)
        : name(l_path.filename().string()), m_sharedContext{ l_sharedContext }
    {
        auto [textureID, textureHandle] = m_sharedContext->graphics->MakeTexture(l_path.string().c_str());
        ID                              = textureID;
        handle                          = textureHandle;
    }

    Texture(const Texture&)            = delete; // TODO what operations do i need?
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other)            = delete;
    Texture& operator=(Texture&& other) = delete;

    ~Texture()
    {
        // std::cout << "Texture gone ID: " << ID << " name: " << name << '\n';
        if (ID != 0)
        {
            m_sharedContext->graphics->FreeTexture(ID, handle);
        }
    }
    std::string name;

    GLuint ID;
    GLuint64 handle = 0;

private:
    SharedContext* m_sharedContext;
};