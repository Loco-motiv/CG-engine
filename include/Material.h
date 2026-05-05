#pragma once

#include "ResourceManager.h"
#include "TextureManager.h"

#include <fstream>

class Material // TODO i need instance handling
{
public:
    Material(std::string l_name, std::string l_diffuseTexture, std::string l_specularTexture, std::string l_alphaTexture, std::string l_normalTexture, GLuint l_shininess, GLfloat l_alpha, sf::Vector3f l_ambientColour, sf::Vector3f l_diffuseColour, sf::Vector3f l_specularColour, SharedContext* l_sharedContext)
        : name(l_name), shininess(l_shininess), alpha(l_alpha), ambientColour(l_ambientColour), diffuseColour(l_diffuseColour), specularColour(l_specularColour), m_sharedContext{ l_sharedContext }
    {
        if (l_diffuseTexture.empty())
        {
            diffuseTexture = nullptr;
        }
        else
        {
            diffuseTexture = m_sharedContext->textureManager->Get(l_diffuseTexture);
        }
        if (l_specularTexture.empty())
        {
            specularTexture = nullptr;
        }
        else
        {
            specularTexture = m_sharedContext->textureManager->Get(l_specularTexture);
        }
        if (l_alphaTexture.empty())
        {
            alphaTexture = nullptr;
        }
        else
        {
            alphaTexture = m_sharedContext->textureManager->Get(l_alphaTexture);
        }
        if (l_normalTexture.empty())
        {
            normalTexture = nullptr;
        }
        else
        {
            normalTexture = m_sharedContext->textureManager->Get(l_normalTexture);
        }
    }

    Material(const Material&)            = default;
    Material& operator=(const Material&) = default;

    Material(Material&& other)            = delete;
    Material& operator=(Material&& other) = delete;

    ~Material()
    {
        // std::cout << "Material gone: " << name << '\n';
    }

    std::string name;

    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> specularTexture;
    std::shared_ptr<Texture> alphaTexture;
    std::shared_ptr<Texture> normalTexture;

    GLuint shininess            = 32;
    GLfloat alpha               = 1.0f;
    sf::Vector3f ambientColour  = sf::Vector3f(0.0f, 0.0f, 0.0f);
    sf::Vector3f diffuseColour  = sf::Vector3f(0.8f, 0.8f, 0.8f);
    sf::Vector3f specularColour = sf::Vector3f(1.0f, 1.0f, 1.0f);

    SharedContext* m_sharedContext;
};
