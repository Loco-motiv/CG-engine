#include "Renderer.h"

Renderer::Renderer(SharedContext* l_sharedContext)
    : m_sharedContext{ l_sharedContext } {}

Renderer::~Renderer() {}

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
        shader->SetDepthCubemapTexture(m_sharedContext->graphics->GetShadowMapTexture());
        shader->SetFloatVec3("viewPosition", l_cameraPosition.x, l_cameraPosition.y, l_cameraPosition.z);

        Material* material = elem->GetMaterial();

        shader->SetFloatMatrix("modelMatrix", elem->GetModelMatrix().GetArray());
        shader->SetFloatMatrix("transformMatrix", elem->GetTransformMatrix().GetArray());

        shader->SetFloatVec3("material.ambientColour", material->ambientColour.x, material->ambientColour.y, material->ambientColour.z);
        shader->SetFloatVec3("material.diffuseColour", material->diffuseColour.x, material->diffuseColour.y, material->diffuseColour.z);
        shader->SetFloatVec3("material.specularColour", material->specularColour.x, material->specularColour.y, material->specularColour.z);
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
        if (material->alphaTexture != nullptr)
        {
            shader->SetAlphaTexture(material->alphaTexture->ID);
        }
        if (material->normalTexture != nullptr)
        {
            shader->SetNormalTexture(material->normalTexture->ID);
        }
        shader->SetBool("material.hasDiffuseTexture", material->diffuseTexture != nullptr);
        shader->SetBool("material.hasSpecularTexture", material->specularTexture != nullptr);
        shader->SetBool("material.hasAlphaTexture", material->alphaTexture != nullptr);
        shader->SetBool("material.hasNormalTexture", material->normalTexture != nullptr);

        if (elem->GetLight().type != LightType::None)
        {
            shader->SetFloatVec3("material.diffuseColour", elem->GetLight().specular.x, elem->GetLight().specular.y, elem->GetLight().specular.z);
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
                Mesh* quadMesh = m_sharedContext->meshManager->Get("Quad").get();
                m_sharedContext->graphics->DrawMesh(quadMesh->VAO, quadMesh->elementsCount);

                x += (ch.advance >> 6) * elem.scale / maxTextHeight;
            }
        }
    }
    glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderObjectsPT(sf::Vector3f cameraPosition, GLfloat FOV, sf::Vector3f forward, sf::Vector3f right, sf::Vector3f up, GLint frameCount)
{
    m_ptShader->Use();
    m_ptShader->SetFloatVec3("cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);

    sf::Vector2u winSize = m_sharedContext->window->GetWindowSize();
    float aspectRatio    = (float)winSize.x / (float)winSize.y;

    float fovRadians     = FOV * (3.14159265f / 180.0f);
    float viewportHeight = 2.0f * tan(fovRadians / 2.0f);
    float viewportWidth  = aspectRatio * viewportHeight;

    sf::Vector3f rayRight = right * viewportWidth;
    sf::Vector3f rayUp    = up * viewportHeight;

    sf::Vector3f rayBottomLeft = forward - (rayRight / 2.0f) - (rayUp / 2.0f);

    m_ptShader->SetFloatVec3("cameraRayBottomLeft", rayBottomLeft.x, rayBottomLeft.y, rayBottomLeft.z);
    m_ptShader->SetFloatVec3("cameraRayRight", rayRight.x, rayRight.y, rayRight.z);
    m_ptShader->SetFloatVec3("cameraRayUp", rayUp.x, rayUp.y, rayUp.z);
    m_ptShader->SetFloatVec3("prevCameraPosition", m_prevCameraPosition.x, m_prevCameraPosition.y, m_prevCameraPosition.z);
    m_ptShader->SetFloatVec3("prevCameraRayBottomLeft", m_prevCameraRayBottomLeft.x, m_prevCameraRayBottomLeft.y, m_prevCameraRayBottomLeft.z);
    m_ptShader->SetFloatVec3("prevCameraRayRight", m_prevCameraRayRight.x, m_prevCameraRayRight.y, m_prevCameraRayRight.z);
    m_ptShader->SetFloatVec3("prevCameraRayUp", m_prevCameraRayUp.x, m_prevCameraRayUp.y, m_prevCameraRayUp.z);
    m_ptShader->SetInt("frameCount", frameCount);

    if (m_sharedContext->graphics->GetComputeTextureID() == 0)
    {
        m_sharedContext->graphics->MakeComputeTexture(m_sharedContext->window->GetWindowSize().x, m_sharedContext->window->GetWindowSize().y);
    }
    m_sharedContext->graphics->BindComputeTexture();
    m_sharedContext->graphics->BindAndSwapReservoirs(5, 6);
    GLuint numGroupsX = (winSize.x + 7) / 8;
    GLuint numGroupsY = (winSize.y + 7) / 8;

    glDispatchCompute(numGroupsX, numGroupsY, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    if (m_screenQuadShader == nullptr)
    {
        m_screenQuadShader = m_sharedContext->shaderManager->Get("Screen.txt");
    }
    m_screenQuadShader->Use();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_sharedContext->graphics->GetComputeTextureID());
    m_screenQuadShader->SetInt("screenTexture", 0);

    Mesh* quadMesh = m_sharedContext->meshManager->Get("Quad").get();
    m_sharedContext->graphics->DrawMesh(quadMesh->VAO, quadMesh->elementsCount);
    m_prevCameraPosition      = cameraPosition;
    m_prevCameraRayBottomLeft = rayBottomLeft;
    m_prevCameraRayRight      = rayRight;
    m_prevCameraRayUp         = rayUp;
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

Shader* Renderer::SetPTShader(std::string l_name)
{
    if (m_ptShader and m_ptShader->name == l_name)
    {
        return m_ptShader.get();
    }
    m_ptShader = m_sharedContext->shaderManager->Get(l_name);
    return m_ptShader.get();
}

bool Renderer::UpdateLightData(Object* l_object, int l_pointLightCount, int l_spotLightCount)
{
    LightComponent m_light  = l_object->GetLight();
    sf::Vector3f m_position = l_object->GetPosition();

    auto shader = l_object->GetShader();
    if (m_light.type == LightType::None)
    {
        return false;
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
    return true;
}

void Renderer::UpdateLightData(const std::vector<std::unique_ptr<Object>>& l_objects)
{
    int pointLightCount = 0;
    int spotLightCount  = 0;
    std::vector<PointLightGPU> gpuLights;

    if (m_shadowShader == nullptr)
    {
        m_shadowShader = m_sharedContext->shaderManager->Get("Shadow.txt");
    }

    float farPlane = 10000.0f;
    MatrixFloat shadowProj;
    shadowProj.PerspectiveProjection(90.0f, 1.0f, 0.1f, farPlane);

    m_sharedContext->graphics->BeginShadowMapDraw();
    m_shadowShader->Use();

    int shadowMapIndex = 0;

    for (const auto& elem : l_objects)
    {
        LightComponent light = elem->GetLight();
        if (light.type == LightType::Point)
        {
            pointLightCount++;
            PointLightGPU pl;
            pl.position  = elem->GetPosition();
            pl.constant  = light.constant;
            pl.linear    = light.linear;
            pl.quadratic = light.quadratic;
            pl.farPlane  = farPlane;

            pl.shadowIndex = -1;

            if (shadowMapIndex < 50)
            {
                pl.shadowIndex = shadowMapIndex;

                m_shadowShader->SetInt("lightIndex", shadowMapIndex);
                m_shadowShader->SetFloatVec3("lightPos", pl.position.x, pl.position.y, pl.position.z);
                m_shadowShader->SetFloat("farPlane", farPlane);

                sf::Vector3f lightPos = pl.position;
                struct Face
                {
                    sf::Vector3f target;
                    sf::Vector3f up;
                };
                Face faces[6] = {
                    {  { 1, 0, 0 }, { 0, -1, 0 } },
                    { { -1, 0, 0 }, { 0, -1, 0 } },
                    {  { 0, 1, 0 },  { 0, 0, 1 } },
                    { { 0, -1, 0 }, { 0, 0, -1 } },
                    {  { 0, 0, 1 }, { 0, -1, 0 } },
                    { { 0, 0, -1 }, { 0, -1, 0 } }
                };

                for (int i = 0; i < 6; ++i)
                {
                    MatrixFloat view;
                    view.LookAt(lightPos, lightPos + faces[i].target, faces[i].up);
                    MatrixFloat shadowTransform = shadowProj * view;
                    m_shadowShader->SetFloatMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransform.GetArray());
                }

                for (const auto& meshObj : l_objects)
                {
                    if (meshObj->GetMesh() == nullptr || meshObj->GetLight().type != LightType::None)
                    {
                        continue;
                    }
                    m_shadowShader->SetFloatMatrix("modelMatrix", meshObj->GetModelMatrix().GetArray());
                    Material* material = elem->GetMaterial();
                    if (material->diffuseTexture != nullptr)
                    {
                        m_shadowShader->SetDiffTexture(material->diffuseTexture->ID);
                    }
                    if (material->alphaTexture != nullptr)
                    {
                        m_shadowShader->SetAlphaTexture(material->alphaTexture->ID);
                    }
                    m_shadowShader->SetBool("hasDiffuseTexture", material->diffuseTexture != nullptr);
                    m_shadowShader->SetBool("hasAlphaTexture", material->alphaTexture != nullptr);
                    m_sharedContext->graphics->DrawMesh(meshObj->GetMesh()->VAO, meshObj->GetMesh()->elementsCount);
                }

                shadowMapIndex++;
            }

            pl.ambient  = light.ambient;
            pl.diffuse  = light.diffusive;
            pl.specular = light.specular;

            gpuLights.push_back(pl);
        }
        else if (light.type == LightType::Spot)
        {
            spotLightCount++;
        }

        if (light.type != LightType::None && light.hasChanges)
        {
            UpdateLightData(elem.get(), pointLightCount, spotLightCount);
        }
    }

    m_sharedContext->graphics->EndShadowMapDraw(m_sharedContext->window->GetWindowSize().x, m_sharedContext->window->GetWindowSize().y);

    if (!gpuLights.empty())
    {
        m_sharedContext->graphics->SetSSBOData(0, gpuLights.size() * sizeof(PointLightGPU), gpuLights.data(), GL_DYNAMIC_DRAW);
    }
}