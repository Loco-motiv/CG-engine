#pragma once

#include "ResourceManager.h"
#include "TextureManager.h"

#include <fstream>

class Material // TODO i need instance handling
{
public:
    Material(const fs::path& l_path, SharedContext* l_sharedContext)
        : name(l_path.filename().string()), m_sharedContext{ l_sharedContext }
    {
        std::ifstream inputStream(l_path);
        if (!inputStream.is_open())
        {
            std::cout << "ERROR::MATERIAL_FILE: failed to open " << l_path << '\n';
        }
        else
        {
            std::string textureName;
            if (inputStream >> textureName)
            {
                diffuseTexture = m_sharedContext->textureManager->Get(textureName);
            }
            if (inputStream >> textureName)
            {
                specularTexture = m_sharedContext->textureManager->Get(textureName);
            }
            if (inputStream >> shininess >> alpha >> colour.x >> colour.y >> colour.z)
            {
            }
        }
    }

    Material(const Material&)            = default;
    Material& operator=(const Material&) = default;

    Material(Material&& other)            = delete;
    Material& operator=(Material&& other) = delete;

    ~Material()
    {
        std::cout << "Material gone: " << name << '\n';
    }

    std::string name;

    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> specularTexture;

    GLuint shininess    = 32;
    GLfloat alpha       = 1.0f;
    sf::Vector3f colour = sf::Vector3f(1.0f, 1.0f, 1.0f); //* defaults to white

    SharedContext* m_sharedContext;
};
