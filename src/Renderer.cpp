#include "Renderer.h"

Renderer::Renderer(SharedContext* l_sharedContext)
    : m_sharedContext{ l_sharedContext }
{
    m_rectangleMesh = m_sharedContext->graphics->MakeQuadMesh();
}

Renderer::~Renderer()
{
    m_sharedContext->graphics->FreeMesh(std::get<0>(m_rectangleMesh), std::get<1>(m_rectangleMesh), std::get<2>(m_rectangleMesh));
}

void Renderer::RenderObjects(const std::vector<std::unique_ptr<Object>>& l_objects, sf::Vector3f l_cameraPosition)
{
    for (const auto& elem : l_objects)
    {
        if (elem->GetMesh() == nullptr)
        {
            continue;
        }
        auto shader = elem->GetShader();
        shader->Use();
        shader->SetFloatVec3("viewPosition", l_cameraPosition.x, l_cameraPosition.y, l_cameraPosition.z);

        Material* material = elem->GetMaterial();

        shader->SetFloatMatrix("modelMatrix", elem->GetModelMatrix().GetArray());
        shader->SetFloatMatrix("transformMatrix", elem->GetTransformMatrix().GetArray());

        shader->SetFloatVec3("material.colour", material->colour.x, material->colour.y, material->colour.z);
        shader->SetFloat("material.shininess", material->shininess);
        shader->SetFloat("material.alpha", material->alpha);

        if (material->diffuseTexture != nullptr)
        {
            shader->SetDiffTexture(material->diffuseTexture->ID);
        }
        if (material->specularTexture != nullptr)
        {
            shader->SetSpecTexture(material->specularTexture->ID);
        }
        shader->SetBool("material.hasDiffuseTexture", material->diffuseTexture != nullptr);
        shader->SetBool("material.hasSpecularTexture", material->specularTexture != nullptr);

        if (elem->GetLight().type != LightType::None)
        {
            shader->SetFloatVec3("material.colour", elem->GetLight().specular.x, elem->GetLight().specular.y, elem->GetLight().specular.z);
            shader->SetBool("material.isLightSource", true);
        }
        else
        {
            shader->SetBool("material.isLightSource", false);
        }

        m_sharedContext->graphics->DrawMesh(elem->GetMesh()->VAO, elem->GetMesh()->elementsCount);
    }
}

void Renderer::RenderWidgets(const std::vector<std::unique_ptr<Widget>>& l_widgets)
{
    glDisable(GL_DEPTH_TEST); //* GUI always draws on top of everything
    for (const auto& widget : l_widgets)
    {
        for (const auto& elem : widget->GetMeshes())
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
        for (const auto& elem : widget->GetTexts())
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
    }
    glEnable(GL_DEPTH_TEST);
}

sf::Vector2f Renderer::GetTextDimensions(const WidgetText& text)
{
    GLfloat x            = 0;
    GLfloat maxTexHeight = m_sharedContext->graphics->GetMaxTextHeight();
    std::string::const_iterator c;
    for (c = text.text.begin(); c != text.text.end(); c++)
    {
        Character ch = m_sharedContext->graphics->GetCharacter(*c);

        x += (ch.advance >> 6) * text.scale / maxTexHeight; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    return sf::Vector2f(x, 1.0f);
}

void Renderer::RenderObjectsPicking(const std::vector<std::unique_ptr<Object>>& l_objects)
{
    m_sharedContext->graphics->BeginPickingDraw();

    if (!m_pickingShader)
    {
        m_pickingShader = m_sharedContext->shaderManager->Get("Picking.txt");
    }
    m_pickingShader->Use();

    for (const auto& elem : l_objects)
    {
        if (elem->GetMesh() != nullptr) // TODO make picking meshless objects possible
        {
            m_pickingShader->SetFloatMatrix("transformMatrix", elem->GetTransformMatrix().GetArray());
            m_pickingShader->SetUInt("objectID", elem->GetID());

            m_sharedContext->graphics->DrawMesh(elem->GetMesh()->VAO, elem->GetMesh()->elementsCount);
        }
    }

    m_sharedContext->graphics->EndPickingDraw(m_sharedContext->window->GetWindowSize().x, m_sharedContext->window->GetWindowSize().y);
}

void Renderer::ReadPickingPixel(GLuint l_x, GLuint l_y)
{
    m_sharedContext->graphics->ReadPixel(l_x, l_y);
}

void Renderer::UpdateLightData(Object* l_object, int l_pointLightCount, int l_spotLightCount)
{
    LightComponent m_light  = l_object->GetLight();
    sf::Vector3f m_position = l_object->GetPosition();

    auto shader = l_object->GetShader();
    if (m_light.type == LightType::None)
    {
        return;
    }

    if (m_light.type == LightType::Directional)
    {
        shader->SetFloatVec3("dirLight.direction", m_light.direction.x, m_light.direction.y, m_light.direction.z);
        shader->SetFloatVec3("dirLight.ambient", m_light.ambient.x, m_light.ambient.y, m_light.ambient.z);
        shader->SetFloatVec3("dirLight.diffuse", m_light.diffusive.x, m_light.diffusive.y, m_light.diffusive.z);
        shader->SetFloatVec3("dirLight.specular", m_light.specular.x, m_light.specular.y, m_light.specular.z);
    }
    else if (m_light.type == LightType::Point)
    {
        std::string prefix = "pointLights[" + std::to_string(l_pointLightCount - 1) + "].";
        shader->SetFloatVec3(prefix + "position", m_position.x, m_position.y, m_position.z);
        shader->SetFloat(prefix + "constant", m_light.constant);
        shader->SetFloat(prefix + "linear", m_light.linear);
        shader->SetFloat(prefix + "quadratic", m_light.quadratic);
        shader->SetFloatVec3(prefix + "ambient", m_light.ambient.x, m_light.ambient.y, m_light.ambient.z);
        shader->SetFloatVec3(prefix + "diffuse", m_light.diffusive.x, m_light.diffusive.y, m_light.diffusive.z);
        shader->SetFloatVec3(prefix + "specular", m_light.specular.x, m_light.specular.y, m_light.specular.z);
    }
    else if (m_light.type == LightType::Spot)
    {
        std::string prefix = "spotLight[" + std::to_string(l_spotLightCount - 1) + "].";
        shader->SetFloatVec3(prefix + "position", m_position.x, m_position.y, m_position.z);
        shader->SetFloatVec3(prefix + "direction", m_light.direction.x, m_light.direction.y, m_light.direction.z);
        shader->SetFloat(prefix + "cutOff", m_light.cutOff);
        shader->SetFloat(prefix + "outerCutOff", m_light.outerCutOff);
        shader->SetFloat(prefix + "constant", m_light.constant);
        shader->SetFloat(prefix + "linear", m_light.linear);
        shader->SetFloat(prefix + "quadratic", m_light.quadratic);
        shader->SetFloatVec3(prefix + "ambient", m_light.ambient.x, m_light.ambient.y, m_light.ambient.z);
        shader->SetFloatVec3(prefix + "diffuse", m_light.diffusive.x, m_light.diffusive.y, m_light.diffusive.z);
        shader->SetFloatVec3(prefix + "specular", m_light.specular.x, m_light.specular.y, m_light.specular.z);
    }
    l_object->SetHasChangesFlag(false);

    shader->SetInt("pointLightCount", l_pointLightCount);
    shader->SetInt("spotLightCount", l_spotLightCount);
}