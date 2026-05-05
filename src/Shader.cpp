#include "Shader.h"

Shader::Shader()
{
}

Shader::Shader(const fs::path& l_path, SharedContext* l_sharedContext)
    : name(l_path.filename().string()), m_sharedContext{ l_sharedContext }
{
    std::ifstream inputStream(l_path);
    if (!inputStream.is_open())
    {
        std::cout << "ERROR::SHADERFILE: failed to open " << l_path << '\n';
        return;
    }
    std::string firstWord;
    inputStream >> firstWord;

    if (firstWord == "COMPUTE")
    {
        std::string compPath;
        inputStream >> compPath;
        CreateCompute(("shaders/" + compPath).c_str());
    }
    else
    {
        std::string vertShaderName = "shaders/" + firstWord;
        std::string fragShaderName, geomShaderName;
        inputStream >> fragShaderName;
        fragShaderName = "shaders/" + fragShaderName;

        if (inputStream >> geomShaderName)
        {
            Create(vertShaderName.c_str(), fragShaderName.c_str(), ("shaders/" + geomShaderName).c_str());
        }
        else
        {
            Create(vertShaderName.c_str(), fragShaderName.c_str(), nullptr);
        }
    }
}

void Shader::Create(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
    auto LoadFile = [](const GLchar* path) -> std::string
    {
        try
        {
            std::ifstream file(path);
            std::stringstream stream;
            stream << file.rdbuf();
            return stream.str();
        }
        catch (...)
        {
            return "";
        }
    };

    std::string vertexCode   = LoadFile(vertexPath);
    std::string fragmentCode = LoadFile(fragmentPath);
    std::string geometryCode = geometryPath ? LoadFile(geometryPath) : "";

    const GLchar* vCode = vertexCode.c_str();
    GLuint vertexID     = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vCode, NULL);
    glCompileShader(vertexID);
    CheckCompileErrors(vertexID, "VERTEX");

    const GLchar* fCode = fragmentCode.c_str();
    GLuint fragmentID   = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fCode, NULL);
    glCompileShader(fragmentID);
    CheckCompileErrors(fragmentID, "FRAGMENT");

    GLuint geometryID = 0;
    if (geometryPath)
    {
        const GLchar* gCode = geometryCode.c_str();
        geometryID          = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryID, 1, &gCode, NULL);
        glCompileShader(geometryID);
        CheckCompileErrors(geometryID, "GEOMETRY");
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    if (geometryPath)
    {
        glAttachShader(ID, geometryID);
    }

    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    if (geometryPath)
    {
        glDeleteShader(geometryID);
    }
}

void Shader::CreateCompute(const GLchar* computePath)
{
    auto LoadFile = [](const GLchar* path) -> std::string
    {
        std::ifstream file(path);
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    };

    std::string computeCode = LoadFile(computePath);
    const GLchar* cCode     = computeCode.c_str();

    GLuint computeID = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeID, 1, &cCode, NULL);
    glCompileShader(computeID);
    CheckCompileErrors(computeID, "COMPUTE");

    ID = glCreateProgram();
    glAttachShader(ID, computeID);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(computeID);
}

Shader::~Shader()
{
    // std::cout << "Shader gone ID: " << ID << " name: " << name << '\n';
    if (ID != 0)
    {
        glDeleteProgram(ID);
    }
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string& name, const GLboolean value) const
{
    glProgramUniform1i(ID, GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, const GLint value) const
{
    glProgramUniform1i(ID, GetUniformLocation(name), value);
}

void Shader::SetUInt(const std::string& name, const GLuint value) const
{
    glProgramUniform1ui(ID, GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, const GLfloat value) const
{
    glProgramUniform1f(ID, GetUniformLocation(name), value);
}

void Shader::SetFloatVec2(const std::string& name, const GLfloat valueX, const GLfloat valueY) const
{
    glProgramUniform2f(ID, GetUniformLocation(name), valueX, valueY);
}

void Shader::SetFloatVec3(const std::string& name, const GLfloat valueX, const GLfloat valueY, const GLfloat valueZ) const
{
    glProgramUniform3f(ID, GetUniformLocation(name), valueX, valueY, valueZ);
}

void Shader::SetFloatVec4(const std::string& name, const GLfloat valueX, const GLfloat valueY, const GLfloat valueZ, const GLfloat valueW) const
{
    glProgramUniform4f(ID, GetUniformLocation(name), valueX, valueY, valueZ, valueW);
}

void Shader::SetFloatMatrix(const std::string& name, const GLfloat* matrix) const
{
    glProgramUniformMatrix4fv(ID, GetUniformLocation(name), 1, GL_FALSE, matrix);
}

void Shader::SetDiffTexture(const GLuint diffusive) const
{
    glBindTextureUnit(1, diffusive);
}

void Shader::SetSpecTexture(const GLuint specular) const
{
    glBindTextureUnit(2, specular);
}

void Shader::SetAlphaTexture(const GLuint alpha) const
{
    glBindTextureUnit(3, alpha);
}

void Shader::SetNormalTexture(const GLuint normal) const
{
    glBindTextureUnit(4, normal);
}

void Shader::SetDepthCubemapTexture(const GLuint depthCubemap) const
{
    glBindTextureUnit(5, depthCubemap);
}

void Shader::SetDiffAndSpecTextures(const GLuint diffusive, const GLuint specular) const
{
    glBindTextureUnit(1, diffusive);
    glBindTextureUnit(2, specular);
}

void Shader::Dispatch(GLuint x, GLuint y, GLuint z) const
{
    glUseProgram(ID);
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) const
{
    GLint success;
    GLint length;
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                GLchar* info(new GLchar[length]);
                glGetShaderInfoLog(shader, length, 0, info);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << "program name: " << name
                          << info << " -- --------------------------------------------------- -- " << std::endl;
                delete[] info;
            }
            else
            {
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << "program name: " << name
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                GLchar* info(new GLchar[length]);
                glGetProgramInfoLog(shader, length, 0, info);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "program name: " << name
                          << info << " -- --------------------------------------------------- -- " << std::endl;
                delete[] info;
            }
            else
            {
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "program name: " << name
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}

GLint Shader::GetUniformLocation(const std::string& l_name) const
{
    if (m_uniformLocations.contains(l_name))
    {
        return m_uniformLocations.at(l_name);
    }
    GLint uniformLocation = glGetUniformLocation(ID, l_name.c_str());
    if (uniformLocation == -1)
    {
        std::cout << "ERROR::SET_VALUE: " << l_name << " no such uniform value in program " << name
                  << "\n -- --------------------------------------------------- -- " << std::endl;
        return -1;
    }
    m_uniformLocations[l_name] = uniformLocation;
    return uniformLocation;
}