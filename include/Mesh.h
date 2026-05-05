#pragma once
#include "ResourceManager.h"

class Mesh
{
public:
    Mesh(std::string l_name, MeshRawData&& l_data, SharedContext* l_sharedContext)
        : name(l_name),
          VAO(l_data.VAO), VBO(l_data.VBO), IBO(l_data.IBO),
          elementsCount(l_data.elementsCount),
          m_vertices(std::move(l_data.vertices)),
          m_indices(std::move(l_data.indices)),
          m_sharedContext{ l_sharedContext } {}
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other)            = delete;
    Mesh& operator=(Mesh&& other) = delete;

    ~Mesh()
    {
        // std::cout << "Mesh gone ID: " << VAO << " name: " << name << '\n';

        if (VAO != 0)
        {
            m_sharedContext->graphics->FreeMesh(VAO, VBO, IBO);
        }
    }
    std::string name;

    GLuint VAO, VBO, IBO;
    GLuint elementsCount;
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

private:
    SharedContext* m_sharedContext;
};
