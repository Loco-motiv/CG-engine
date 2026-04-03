#pragma once

#include "Graphics.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Object.h"
#include "ShaderManager.h"
#include "SharedContext.h"
#include "TextureManager.h"
#include "Window.h"

class Renderer
{
public:
    Renderer(SharedContext* l_sharedContext);
    ~Renderer();

    void RenderObjects(const std::vector<std::unique_ptr<Object>>& l_objects, sf::Vector3f l_cameraPosition);
    void RenderTextGUI(const std::string& text, GLfloat x, GLfloat y,
                       GLfloat sx, GLfloat sy, GLfloat scale,
                       GLfloat colorR, GLfloat colorG, GLfloat colorB, GLfloat alpha);
    void RenderGUI(GLfloat x, GLfloat y, GLfloat sx, GLfloat sy, GLfloat colorR, GLfloat colorG, GLfloat colorB, GLfloat alpha);
    void RenderObjectsPicking(const std::vector<std::unique_ptr<Object>>& l_objects);
    void ReadPickingPixel(GLuint l_x, GLuint l_y);

    void UpdateLightData(Object* l_object, int pointLightCount, int spotLightCount);

private:
    SharedContext* m_sharedContext;

    std::shared_ptr<Shader> m_pickingShader;
    std::shared_ptr<Shader> m_GUIShader;

    std::tuple<GLuint, GLuint, GLuint, GLuint> m_rectangleMesh;
};