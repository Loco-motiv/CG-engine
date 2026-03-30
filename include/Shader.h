#pragma once

#include "SharedContext.h"

#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

class Shader
{
public:
    GLuint ID;
    std::string name;

    Shader();
    Shader(const fs::path& l_path, SharedContext* l_sharedContext);
    void Create(const GLchar* vertexPath, const GLchar* fragmentPath);

    ~Shader();

    Shader(const Shader&)             = delete;
    Shader& operator=(const Shader&)  = delete;
    Shader(Shader&& other)            = delete;
    Shader& operator=(Shader&& other) = delete;

    void Use() const;

    void SetBool(const std::string& name, const GLboolean value) const;
    void SetInt(const std::string& name, const GLint value) const;
    void SetUInt(const std::string& name, const GLuint value) const;
    void SetFloat(const std::string& name, const GLfloat value) const;
    void SetFloatVec2(const std::string& name, const GLfloat valueX, const GLfloat valueY) const;
    void SetFloatVec3(const std::string& name, const GLfloat valueX, const GLfloat valueY, const GLfloat valueZ) const;
    void SetFloatMatrix(const std::string& name, const GLfloat* matrix) const;

    void SetDiffTexture(const GLuint diffusive) const;
    void SetSpecTexture(const GLuint specular) const;
    void SetDiffAndSpecTextures(const GLuint diffusive, const GLuint specular) const;

private:
    void CheckCompileErrors(GLuint shader, const std::string& type) const;
    GLint GetUniformLocation(const std::string& name) const;

    SharedContext* m_sharedContext;

    mutable std::unordered_map<std::string, GLint> m_uniformLocations;
};