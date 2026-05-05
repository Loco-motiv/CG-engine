#pragma once

#include "Matrix.h"
#include "Shader.h"
#include "Vector.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stb_image.h"
#include "tiny_obj_loader.h"

#include <SFML/Window.hpp>
#include <map>
#include <unordered_map>

struct Character
{
    GLuint textureID; // ID handle of the glyph texture
    GLuint sizeX;
    GLuint sizeY;
    GLint bearingLeft; // Offset from baseline to left/top of glyph
    GLint bearingTop;
    GLint advance;     // Horizontal offset to advance to next glyph
};

struct Vertex
{
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat u, v;
    GLfloat tx, ty, tz;

    bool operator==(const Vertex& other) const
    {
        return memcmp(this, &other, sizeof(Vertex)) == 0;
    }
};

struct MeshRawData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GLuint VAO = 0, VBO = 0, IBO = 0;
    size_t elementsCount = 0;
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    std::pair<GLuint, GLuint64> MakeTexture(char const* path);
    void FreeTexture(GLuint l_id, GLuint64 l_handle);

    MeshRawData MakeQuadMesh();
    std::vector<std::pair<std::string, MeshRawData>> MakeMeshes(const std::string& path, const std::string& l_matPath);
    GLuint MakeComputeTexture(GLuint width, GLuint height);

    std::vector<tinyobj::material_t> GetMeshMaterials(const std::string& path);
    std::vector<std::pair<std::string, std::string>> GetMeshAndMaterialNames(const std::string& path, const std::string& l_matPath);
    void FreeMesh(GLuint l_VAO, GLuint l_VBO, GLuint l_IBO);
    void DrawMesh(GLuint l_meshID, GLuint elementsCount);

    Character GetCharacter(char c) const;
    GLfloat GetMaxTextHeight() const;

    void BeginPickingDraw();
    void EndPickingDraw(GLuint l_width, GLuint l_height);
    void ReadPixel(GLuint x, GLuint y);
    GLint GetPickingResult();

    void SetSSBOData(GLuint bindingPoint, GLsizeiptr size, const void* data, GLenum usage);
    void BindAndSwapReservoirs(GLuint firstBindPoint, GLuint secondBindPoint);

    void BeginShadowMapDraw();
    void EndShadowMapDraw(GLuint l_width, GLuint l_height);
    GLuint GetShadowMapTexture() const;

    void BindComputeTexture();
    GLuint GetComputeTextureID() const { return m_computeTextureID; }

    GLuint m_pickingWidth  = 1920;
    GLuint m_pickingHeight = 1080;

private:
    void MakePBOs();
    void MakeFBOs();

    void MakePickingFBO(GLuint l_width, GLuint l_height);
    void MakePickingPBO();
    void MakeSSBO();
    void MakeShadowMap();

    void FreeObjects();

    void ConfigureOpenGL();
    void ConfigureFreeType();

    GLuint m_textVAO;
    GLuint m_textVBO;

    GLuint m_pickingColorTexture;
    GLuint m_pickingDepthTexture;
    GLuint m_pickingFBO;
    GLuint m_pickingPBO;
    GLsync m_pickingFence = 0;

    // GLuint pointLightSSBO;
    std::vector<GLuint> SSBOs;
    GLuint maxSSBObindings = 8;

    GLuint m_reservoirA, m_reservoirB;

    GLuint depthCubemapArray;
    GLuint depthFBO;
    GLuint m_shadowRes = 1024;

    GLuint m_computeTextureID = 0;

    FT_Library m_ft;
    FT_Face m_face;
    std::map<GLchar, Character> Characters;

    GLfloat m_maxTextHeight = 32;
};