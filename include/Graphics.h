#pragma once

#include "Matrix.h"
#include "Shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stb_image.h"

#include <SFML/Window.hpp>
#include <map>

struct Character
{
    GLuint textureID; // ID handle of the glyph texture
    GLuint sizeX;
    GLuint sizeY;
    GLint bearingLeft; // Offset from baseline to left/top of glyph
    GLint bearingTop;
    GLint advance;     // Horizontal offset to advance to next glyph
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    Shader* m_shader;
    Shader* m_textShader;
    Shader* m_lightSourceShader;
    Shader* m_GUIShader;
    Shader* m_pickingShader;

    std::vector<std::pair<GLuint, GLuint>> m_textures;

    void DrawCube();
    void DrawLightSourceCube();
    void DrawRectangle();
    void DrawSphere();
    void DrawLightSourceSphere();
    void RenderText(const std::string& text, GLfloat x, GLfloat y,
                    GLfloat sx, GLfloat sy, GLfloat scale,
                    GLfloat colorR, GLfloat colorG, GLfloat colorB);
    sf::Vector2f GetTextDimensions(const std::string& text, GLfloat sx, GLfloat sy, GLfloat scale);
    GLfloat GetMaxTextHeight(GLfloat sy) const;

    void BeginPickingDraw();
    void EndPickingDraw(GLuint l_width, GLuint l_height);
    void ReadPixel(GLuint x, GLuint y);
    GLuint GetPickingResult();

    GLuint m_pickingWidth  = 800;
    GLuint m_pickingHeight = 600;

private:
    void MakeShaders();
    void MakeVAOs();
    void MakeTextures();
    void MakePBOs();
    void MakeFBOs();

    void MakeShader(const GLchar* vertexPath, const GLchar* fragmentPath, Shader*& l_shader);
    GLuint MakeTexture(char const* path);

    void MakeTriangleVAO();
    void MakeCubeVAO();
    void MakeSphereVAO();
    void MakeLightSourceCubeVAO();
    void MakeLightSourceSphereVAO();
    void MakeFullScreenRectangleVAO();
    void MakeRectangleVAO();

    void MakePickingFBO(GLuint l_width, GLuint l_height);

    void MakePickingPBO();

    void FreeObjects();

    void ConfigureOpenGL();
    void ConfigureFreeType();

    GLuint m_triangleVAO;

    GLuint m_cubeVAO;
    GLuint m_lightSourceCubeVAO;
    GLuint m_cubeVBO;
    GLuint m_cubeIBO;

    GLuint m_rectangleVAO;

    GLuint m_sphereVAO;
    GLuint m_lightSourceSphereVAO;
    GLuint m_sphereVBO;
    GLuint m_sphereIBO;

    GLuint m_sphereIndexCount;

    GLuint m_textVAO;
    GLuint m_textVBO;

    GLuint m_fullScreenRectangleVAO;

    GLuint m_pickingColorTexture;
    GLuint m_pickingDepthTexture;
    GLuint m_pickingFBO;
    GLuint m_pickingPBO;
    GLsync m_pickingFence = 0;

    FT_Library m_ft;
    FT_Face m_face;
    std::map<GLchar, Character> Characters;

    GLfloat m_minTextHeight;
    GLfloat m_maxTextHeight;
};