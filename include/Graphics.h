#pragma once

#include "Matrix.h"
#include "Shader.h"

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

class Graphics
{
public:
    Graphics();
    ~Graphics();

    GLuint MakeTexture(char const* path);
    void FreeTexture(GLuint l_id);

    std::tuple<GLuint, GLuint, GLuint, GLuint> MakeMesh(const std::string path);
    void FreeMesh(GLuint l_VAO, GLuint l_VBO, GLuint l_IBO);
    void DrawMesh(GLuint l_meshID, GLuint elementsCount);
    void RenderText(const std::string& text, GLfloat x, GLfloat y,
                    GLfloat sx, GLfloat sy, GLfloat scale,
                    GLfloat colorR, GLfloat colorG, GLfloat colorB);
    sf::Vector2f GetTextDimensions(const std::string& text, GLfloat sx, GLfloat sy, GLfloat scale);
    GLfloat GetMaxTextHeight(GLfloat sy) const;

    void BeginPickingDraw();
    void EndPickingDraw(GLuint l_width, GLuint l_height);
    void ReadPixel(GLuint x, GLuint y);
    GLuint GetPickingResult();

    GLuint m_pickingWidth  = 1920;
    GLuint m_pickingHeight = 1080;

private:
    void MakePBOs();
    void MakeFBOs();

    void MakePickingFBO(GLuint l_width, GLuint l_height);
    void MakePickingPBO();

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

    FT_Library m_ft;
    FT_Face m_face;
    std::map<GLchar, Character> Characters;

    GLfloat m_minTextHeight;
    GLfloat m_maxTextHeight;
};