#pragma once

#include "Graphics.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Object.h"
#include "ShaderManager.h"
#include "SharedContext.h"
#include "TextureManager.h"
#include "Widget.h"
#include "Window.h"

#include <ranges>

class Renderer
{
public:
    Renderer(SharedContext* l_sharedContext);
    ~Renderer();

    void RenderObjects(const std::vector<std::unique_ptr<Object>>& l_objects, sf::Vector3f l_cameraPosition);
    void RenderWidgets(const std::vector<std::unique_ptr<Widget>>& l_widgets);
    void RenderWidgetMeshes(const std::ranges::common_range auto& l_widgetMeshes)
    {
        glDisable(GL_DEPTH_TEST); //* GUI always draws on top of everything
        for (const auto& elem : l_widgetMeshes)
        {
            if (elem.isVisible == false)
            {
                continue;
            }
            if (elem.mesh == nullptr)
            {
                continue;
            }
            if (elem.shader == nullptr)
            {
                continue;
            }
            MatrixFloat transform;
            transform.Move(elem.position.x, elem.position.y, 0.0f);
            transform.ScaleXYZ(elem.scale.x, elem.scale.y, elem.scale.z);
            elem.shader->Use();
            elem.shader->SetFloatMatrix("transformMatrix", transform.GetArray());
            elem.shader->SetFloatVec4("colour", elem.style.colour.x, elem.style.colour.y, elem.style.colour.z, elem.style.alpha);

            elem.shader->SetBool("useTexture", false);

            elem.shader->SetFloatVec4("outlineColour", elem.style.outlineColour.x, elem.style.outlineColour.y, elem.style.outlineColour.z, elem.style.alpha);
            elem.shader->SetFloat("outlineThickness", elem.style.outlineThickness);
            elem.shader->SetFloat("cornerRadius", elem.style.cornerRadius);
            elem.shader->SetFloatVec2("dimensions", elem.dimensions.x, elem.dimensions.y);

            m_sharedContext->graphics->DrawMesh(elem.mesh->VAO, elem.mesh->elementsCount);
        }
        glEnable(GL_DEPTH_TEST);
    }
    void RenderWidgetTexts(const std::ranges::common_range auto& l_widgetTexts)
    {
        glDisable(GL_DEPTH_TEST); //* GUI always draws on top of everything
        for (const auto& elem : l_widgetTexts)
        {
            if (elem.isVisible == false)
            {
                continue;
            }
            if (elem.shader == nullptr)
            {
                continue;
            }
            elem.shader->Use();
            elem.shader->SetInt("useTexture", 1);
            elem.shader->SetFloatVec4("colour", elem.style.colour.x, elem.style.colour.y, elem.style.colour.z, elem.style.alpha);

            GLfloat maxTextHeight = m_sharedContext->graphics->GetMaxTextHeight();
            GLfloat x             = elem.position.x;
            std::string::const_iterator c;
            for (c = elem.text.begin(); c != elem.text.end(); c++)
            {
                Character ch = m_sharedContext->graphics->GetCharacter(*c);

                GLfloat w = ch.sizeX * elem.scale / maxTextHeight;
                GLfloat h = ch.sizeY * elem.scale / maxTextHeight;

                GLfloat xpos = x + ch.bearingLeft * elem.scale / maxTextHeight;
                GLfloat ypos = elem.position.y - (static_cast<int>(ch.sizeY) - ch.bearingTop) * elem.scale / maxTextHeight;

                MatrixFloat charTransform;
                charTransform.ScaleX(w);
                charTransform.ScaleY(h);
                charTransform.Move(xpos + w / 2.0f, ypos + h / 2.0f, 0.0f);

                elem.shader->SetFloatMatrix("transformMatrix", charTransform.GetArray());

                elem.shader->SetDiffTexture(ch.textureID);
                m_sharedContext->graphics->DrawMesh(std::get<0>(m_rectangleMesh), std::get<3>(m_rectangleMesh));

                x += (ch.advance >> 6) * elem.scale / maxTextHeight;
            }
        }
        glEnable(GL_DEPTH_TEST);
    }
    sf::Vector2f GetTextDimensions(const WidgetText& text);
    void RenderObjectsPicking(const std::vector<std::unique_ptr<Object>>& l_objects);
    void ReadPickingPixel(GLuint l_x, GLuint l_y);

    void UpdateLightData(Object* l_object, int pointLightCount, int spotLightCount);

private:
    SharedContext* m_sharedContext;

    std::shared_ptr<Shader> m_pickingShader;
    // std::shared_ptr<Shader> m_GUIShader;

    std::tuple<GLuint, GLuint, GLuint, GLuint> m_rectangleMesh;
};