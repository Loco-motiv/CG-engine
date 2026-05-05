#pragma once
#include "Material.h"
#include "ResourceManager.h"

#include <fstream>

class MaterialManager : public ResourceManager<MaterialManager, Material>
{
public:
    MaterialManager(SharedContext* l_sharedContext, const fs::path& l_materialDirectory)
        : ResourceManager(l_sharedContext, l_materialDirectory) {}

    void InitPreload()
    {
        for (const auto& file : m_paths)
        {
            std::vector<tinyobj::material_t> materials = m_sharedContext->graphics->GetMeshMaterials(file.second.string());
            for (const auto& material : materials)
            {
                m_preLoadedResources[file.first + ":" + material.name] =
                    std::make_shared<Material>(file.first + ":" + material.name, material.diffuse_texname, material.specular_texname,
                                               material.alpha_texname, material.normal_texname,
                                               material.shininess <= 1.0 ? 32 : static_cast<GLuint>(material.shininess), material.dissolve,
                                               sf::Vector3f(material.ambient[0], material.ambient[1], material.ambient[2]),
                                               sf::Vector3f(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
                                               sf::Vector3f(material.specular[0], material.specular[1], material.specular[2]),
                                               m_sharedContext);
                m_names.push_back(file.first + ":" + material.name);
                // std::cout << file.first + ":" + material.name << ' ' << material.diffuse_texname << ' ' << material.specular_texname << ' ' << static_cast<GLuint>(material.shininess) << ' ' << material.dissolve << ' ' << material.diffuse[0] << ' ' << material.diffuse[1] << ' ' << material.diffuse[2] << ' ' << '\n';
            }
        }
        // for (const auto& name : m_names)
        // {
        //     std::cout << name << '\n';
        // }
    }

    std::shared_ptr<Material> Load(const fs::path& l_path)
    {
        return nullptr;
    }
};