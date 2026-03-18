#include "SceneManager.h"

SceneManager::SceneManager(SharedContext* l_sharedContext) : m_sharedContext{ l_sharedContext },
                                                             m_camera(-1, 30.0f, 0.05f, 0.3f,
                                                                      { 0.0f, 0.0f, 0.0f },
                                                                      { 0.0f, 0.0f, 3.0f }, { 1.0f, 1.0f, 1.0f }) {}

SceneManager::~SceneManager()
{
}

Object* SceneManager::MakeCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                               Material l_material)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* cube = m_objects.back().get();
    cube->SetMaterial(l_material);
    cube->SetMesh({ 0, true });
    return cube;
}

Object* SceneManager::MakeSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, Material l_material)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* sphere = m_objects.back().get();
    sphere->SetMaterial(l_material);
    sphere->SetMesh({ 1, true });
    return sphere;
}

Object* SceneManager::MakePointLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* pointLight = m_objects.back().get();
    pointLight->SetLight(l_light);
    return pointLight;
}

Object* SceneManager::MakeLightCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* lightCube = m_objects.back().get();
    lightCube->SetLight(l_light);
    lightCube->SetMesh({ 0, true });
    return lightCube;
}

Object* SceneManager::MakeLightSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* lightSphere = m_objects.back().get();
    lightSphere->SetLight(l_light);
    lightSphere->SetMesh({ 1, true });
    return lightSphere;
}

Object* SceneManager::MakeDirectionalLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* dirLight = m_objects.back().get();
    dirLight->SetLight(l_light);
    return dirLight;
}

Object* SceneManager::MakeSpotLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* spotLight = m_objects.back().get();
    spotLight->SetLight(l_light);
    return spotLight;
}

Object* SceneManager::MakeObject(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, Material l_material, MeshInfo l_mesh, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_position, l_rotation, l_scale));
    Object* object = m_objects.back().get();
    object->SetMaterial(l_material);
    object->SetMesh(l_mesh);
    object->SetLight(l_light);
    return object;
}

void SceneManager::HandleInput()
{
    if (m_pickingCooldown == 0 and sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*m_sharedContext->window->GetWindow());

        GLuint windowWidth  = m_sharedContext->window->GetWindowSize().x;
        GLuint windowHeight = m_sharedContext->window->GetWindowSize().y;

        GLuint pixelX = (GLuint)(((float)mousePos.x / windowWidth) * m_sharedContext->graphics->m_pickingWidth);
        GLuint pixelY = (GLuint)(((float)(windowHeight - 1 - mousePos.y) / windowHeight) * m_sharedContext->graphics->m_pickingHeight);

        sf::Vector2f NDC_for_GUI_check = m_sharedContext->GUI->ConvertScreenCoordinates(sf::Mouse::getPosition(*m_sharedContext->window->GetWindow()));
        if (NDC_for_GUI_check.x < m_sharedContext->GUI->m_leftBorder or
            NDC_for_GUI_check.x > m_sharedContext->GUI->m_rightBorder or
            NDC_for_GUI_check.y < m_sharedContext->GUI->m_elements.back()->m_topBorder - m_sharedContext->GUI->m_elementHeight or
            NDC_for_GUI_check.y > m_sharedContext->GUI->m_elements.front()->m_topBorder)
        {
            m_pickingCords.x  = pixelX;
            m_pickingCords.y  = pixelY;
            m_flagPicked      = true;
            m_pickingCooldown = m_sharedContext->m_cooldownResetValue;
        }
    }

    GLint xDelta = 0;
    GLint yDelta = 0;
    if (!m_flagReleaseMouse)
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(*m_sharedContext->window->GetWindow());
        sf::Vector2i windowCenter  = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2,
                                                  m_sharedContext->window->GetWindowSize().y / 2);
        xDelta                     = mousePosition.x - windowCenter.x;
        yDelta                     = mousePosition.y - windowCenter.y;
        sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) and m_mouseCooldown < m_sharedContext->m_cooldownDeadZone)
    {
        m_mouseCooldown    = m_sharedContext->m_cooldownResetValue;
        m_flagReleaseMouse = !m_flagReleaseMouse;
        (*m_sharedContext->window->GetWindow()).setMouseCursorVisible(m_flagReleaseMouse);
        sf::Vector2i windowCenter = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2,
                                                 m_sharedContext->window->GetWindowSize().y / 2);
        sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
    }

    m_camera.HandleInput(xDelta, yDelta);
}

void SceneManager::Update(GLint l_elapsed)
{
    if (m_pickingCooldown > 0)
    {
        if (m_pickingCooldown - l_elapsed < 0)
        {
            m_pickingCooldown = 0;
        }
        else
        {
            m_pickingCooldown = m_pickingCooldown - l_elapsed;
        }
    }

    if (m_mouseCooldown > 0)
    {
        if (m_mouseCooldown - l_elapsed < 0)
        {
            m_mouseCooldown = 0;
        }
        else
        {
            m_mouseCooldown -= l_elapsed;
        }
    }

    if (m_flagPickedReady == true)
    {
        m_pickedObject = m_sharedContext->graphics->GetPickingResult();
        if (m_pickedObject != 0)
        {
            std::cout << m_pickedObject << '\n';
        }
        m_flagPickedReady = false;
    }

    m_camera.Update(l_elapsed);
}

void SceneManager::Render()
{
    sf::Vector3f m_cameraPosition = m_camera.GetPosition(); // TODO maybe i should do these computations in update...

    m_viewMatrix = m_camera.GetViewMatrix();

    if (m_flagProjection)
    {
        m_projectionMatrix.OrthographicProjection((float)m_sharedContext->window->GetWindowSize().x /
                                                      (float)m_sharedContext->window->GetWindowSize().y,
                                                  1.0f, -1.0f, 1.0f, -1.0f, m_nearDistance, m_rearDistance);
    }
    else
    {
        m_projectionMatrix.PerspectiveProjection(m_camera.GetFOV(),
                                                 (float)m_sharedContext->window->GetWindowSize().x /
                                                     (float)m_sharedContext->window->GetWindowSize().y,
                                                 m_nearDistance, m_rearDistance);
    }
    m_spotLightCount  = 0;
    m_pointLightCount = 0;

    MatrixFloat projectionViewMatrix = m_projectionMatrix * m_viewMatrix;

    m_sharedContext->shader->Use();

    for (auto& elem : m_objects)
    {
        elem->UpdateTransformMatrix(projectionViewMatrix);

        LightType elemType = elem->GetLight().type;

        if (elemType != LightType::None and elem->GetLight().hasChanges)
        {
            UpdateLightData(elem, m_sharedContext->shader, m_pointLightCount, m_spotLightCount);
        }
        if (elemType == LightType::Point)
        {
            m_pointLightCount++;
        }
        else if (elemType == LightType::Spot)
        {
            m_spotLightCount++;
        }
    }

    m_sharedContext->shader->SetInt("pointLightCount", m_pointLightCount);
    m_sharedContext->shader->SetInt("spotLightCount", m_spotLightCount); // TODO maybe i should do these computations in update...

    if (m_flagPicked == true)
    {
        RenderPicking();

        m_sharedContext->graphics->ReadPixel(m_pickingCords.x, m_pickingCords.y);

        m_flagPicked      = false;
        m_flagPickedReady = true;
    }

    auto shader = m_sharedContext->shader;

    shader->Use();
    shader->SetFloatVec3("viewPosition", m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);

    for (const auto& elem : m_objects)
    {
        if (elem->GetMesh().isActive == false)
        {
            continue;
        }

        Material material = elem->GetMaterial();

        shader->SetFloatMatrix("modelMatrix", elem->GetModelMatrix().GetArray());
        shader->SetFloatMatrix("transformMatrix", elem->GetTransformMatrix().GetArray());

        shader->SetFloatVec3("material.colour", material.colour.x, material.colour.y, material.colour.z);
        shader->SetFloat("material.shininess", material.shininess);
        shader->SetFloat("material.alpha", material.alpha);

        if (material.hasDiffuseTexture)
        {
            shader->SetDiffTexture(material.diffuseTexture);
        }
        if (material.hasSpecularTexture)
        {
            shader->SetSpecTexture(material.specularTexture);
        }
        shader->SetBool("material.hasDiffuseTexture", material.hasDiffuseTexture);
        shader->SetBool("material.hasSpecularTexture", material.hasSpecularTexture);

        if (elem->GetLight().type != LightType::None)
        {
            shader->SetFloatVec3("material.colour", elem->GetLight().specular.x, elem->GetLight().specular.y, elem->GetLight().specular.z);
            shader->SetBool("material.isLightSource", true);
        }
        else
        {
            shader->SetBool("material.isLightSource", false);
        }

        m_sharedContext->graphics->DrawMesh(elem->GetMesh().ID);
    }
}

void SceneManager::RenderPicking()
{
    m_sharedContext->graphics->BeginPickingDraw();

    m_sharedContext->pickingShader->Use();

    for (const auto& elem : m_objects)
    {
        if (elem->GetMesh().isActive) // TODO make picking meshless objects possible
        {
            m_sharedContext->pickingShader->SetFloatMatrix("transformMatrix", elem->GetTransformMatrix().GetArray());
            m_sharedContext->pickingShader->SetUInt("objectID", elem->GetID());

            m_sharedContext->graphics->DrawMesh(elem->GetMesh().ID);
        }
    }

    m_sharedContext->graphics->EndPickingDraw(m_sharedContext->window->GetWindowSize().x, m_sharedContext->window->GetWindowSize().y);
}

void SceneManager::UpdateLightData(std::unique_ptr<Object>& l_object, Shader* shader, int pointLightCount, int spotLightCount)
{
    LightComponent m_light  = l_object->GetLight();
    sf::Vector3f m_position = l_object->GetPosition();
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
        std::string prefix = "pointLights[" + std::to_string(pointLightCount) + "].";
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
        std::string prefix = "spotLight[" + std::to_string(spotLightCount) + "].";
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
}
