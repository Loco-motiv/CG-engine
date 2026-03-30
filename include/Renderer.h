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
    void RenderTextGUI();
    void RenderGUI();
    void RenderObjectsPicking(const std::vector<std::unique_ptr<Object>>& l_objects);
    void ReadPickingPixel(GLuint l_x, GLuint l_y);

    void UpdateLightData(Object* l_object, int pointLightCount, int spotLightCount);

private:
    SharedContext* m_sharedContext;

    std::shared_ptr<Shader> m_pickingShader;
};