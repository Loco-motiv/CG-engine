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
    }
    else
    {
        std::string vertShaderName, fragShaderName;
        if (inputStream >> vertShaderName)
        {
            vertShaderName = "shaders/" + vertShaderName;
        }
        if (inputStream >> fragShaderName)
        {
            fragShaderName = "shaders/" + fragShaderName;
        }
        Create(vertShaderName.c_str(), fragShaderName.c_str());
    }
}

void Shader::Create(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& error)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << error.what() << std::endl;
    }

    const GLchar* vertexShaderCode   = vertexCode.c_str();
    const GLchar* fragmentShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    // vertex shader
    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexID);
    CheckCompileErrors(vertexID, "VERTEX");

    // fragment Shader
    GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentID);
    CheckCompileErrors(fragmentID, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

Shader::~Shader()
{
    std::cout << "Shader gone ID: " << ID << " name: " << name << '\n';
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

void Shader::SetDiffAndSpecTextures(const GLuint diffusive, const GLuint specular) const
{
    glBindTextureUnit(1, diffusive);
    glBindTextureUnit(2, specular);
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
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                GLchar* info(new GLchar[length]);
                glGetShaderInfoLog(shader, length, 0, info);
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