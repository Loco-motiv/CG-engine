#pragma once
#include "ResourceManager.h"

class Mesh
{
public:
    Mesh(std::string l_name, std::tuple<GLuint, GLuint, GLuint, GLuint> meshData, SharedContext* l_sharedContext)
        : name(l_name), VAO(std::get<0>(meshData)), VBO(std::get<1>(meshData)), IBO(std::get<2>(meshData)), elementsCount(std::get<3>(meshData)),
          m_sharedContext{ l_sharedContext } {}
    Mesh(const fs::path& l_path, SharedContext* l_sharedContext)
        : name(l_path.filename().string()), m_sharedContext{ l_sharedContext }
    {
        auto result   = m_sharedContext->graphics->MakeMesh(l_path.string());
        VAO           = std::get<0>(result);
        VBO           = std::get<1>(result);
        IBO           = std::get<2>(result);
        elementsCount = std::get<3>(result);
    }
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other)            = delete;
    Mesh& operator=(Mesh&& other) = delete;

    ~Mesh()
    {
        std::cout << "Mesh gone ID: " << VAO << " name: " << name << '\n';

        if (VAO != 0)
        {
            m_sharedContext->graphics->FreeMesh(VAO, VBO, IBO);
        }
    }
    std::string name;

    GLuint VAO, VBO, IBO;
    GLuint elementsCount;

private:
    SharedContext* m_sharedContext;
};
