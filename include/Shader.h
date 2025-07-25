#pragma once

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

class Shader
{
public:
    GLuint ID;

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    // TODO ~Shader glDeleteTextures(1, &texture);
    //*     glDeleteSamplers(1, &sampler);
    //*     glDeleteBuffers(1, &vbo);
    //*     glDeleteVertexArrays(1, &vao);

    void Use() const;

    void SetBool(const std::string& name, const GLboolean value) const;
    void SetInt(const std::string& name, const GLint value) const;
    void SetUInt(const std::string& name, const GLuint value) const;
    void SetFloat(const std::string& name, const GLfloat value) const;
    void SetFloatVec2(const std::string& name, const GLfloat valueX, const GLfloat valueY) const;
    void SetFloatVec3(const std::string& name, const GLfloat valueX, const GLfloat valueY, const GLfloat valueZ) const;
    void SetFloatMatrix(const std::string& name, const GLfloat* matrix) const;

    void SetDiffAndSpecTextures(const GLuint diffusive, const GLuint specular) const;

private:
    void CheckCompileErrors(GLuint shader, const std::string& type) const;
    GLint GetUniformLocation(const std::string& name) const;
};