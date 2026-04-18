#pragma once

#include "Graphics.h"
#include "SharedContext.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <string>

class SharedContext;

namespace fs = std::filesystem;

template <typename Derived, typename T>
class ResourceManager
{
public:
    explicit ResourceManager(SharedContext* l_sharedContext, const fs::path& l_resourceDirectory)
        : m_sharedContext{ l_sharedContext }, m_resourceDirectory{ l_resourceDirectory }
    {
        LoadPaths();
    }

    std::shared_ptr<T> Get(std::string_view l_name)
    {
        std::string name(l_name);

        if (m_preLoadedResources.contains(name))
        {
            return m_preLoadedResources[name];
        }

        if (m_resources.find(name) != m_resources.end() and !m_resources[name].expired())
        {
            return m_resources[name].lock();
        }

        auto path = m_paths.find(name);
        if (path == m_paths.end())
        {
            return nullptr;
            std::cout << "ERROR::RESOURSE_PATH: " << name << " not found" << '\n';
        }

        std::shared_ptr<T> resourse = static_cast<Derived*>(this)->Load(path->second);
        if (resourse)
        {
            m_resources[name] = resourse;
        }

        return resourse;
    }

    fs::path GetPath(std::string_view l_name)
    {
        auto path = m_paths.find(std::string(l_name));
        return (path != m_paths.end() ? path->second : fs::path());
    }

    std::vector<std::string> GetNames()
    {
        std::vector<std::string> names;

        for (auto const& [key, val] : m_paths)
        {
            names.push_back(key);
        }

        for (auto const& [key, val] : m_preLoadedResources)
        {
            names.push_back(key);
        }
        return names;
    }

    void AddPath(std::string_view l_name, const fs::path& l_path)
    {
        m_paths[std::string(l_name)] = l_path;
    }

    void RemovePath(std::string_view l_name)
    {
        m_paths.erase(std::string(l_name));
    }

protected:
    std::shared_ptr<T> Load(const fs::path& l_path)
    {
        return static_cast<Derived*>(this)->Load(l_path);
    }
    SharedContext* m_sharedContext;

    std::map<std::string, std::weak_ptr<T>> m_resources;
    std::map<std::string, std::shared_ptr<T>> m_preLoadedResources;
    std::map<std::string, fs::path> m_paths;

private:
    void LoadPaths()
    {
        if (!fs::exists(m_resourceDirectory) or !fs::is_directory(m_resourceDirectory))
        {
            std::cout << "ERROR::RESOURCE_DIRECTORY: " << m_resourceDirectory << " no such directory exist"
                      << "\n -- --------------------------------------------------- -- \n";
            return;
        } // TODO make names mangle with subdirectories to eliminate rewrite on same file name

        for (const auto& file : fs::recursive_directory_iterator(m_resourceDirectory))
        {
            m_paths[file.path().filename().string()] = file.path();
        }

        for (auto file : m_paths)
        {
            std::cout << file.first << " " << file.second << '\n';
        }
    }

    fs::path m_resourceDirectory;
};