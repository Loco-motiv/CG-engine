#include "SceneManager.h"

//* Object

Object::Object(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale)
    : m_ID{ l_ID }, m_sceneManager{ l_sceneManager }, m_position{ l_position }, m_rotation{ l_rotation }, m_scale{ l_scale } {}

//* ObjectWithTexture

ObjectWithTexture::ObjectWithTexture(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                     Texture l_texture, GLuint l_shininess, GLfloat l_alpha)
    : Object{ l_ID, l_sceneManager, l_position, l_rotation, l_scale }, m_shininess{ l_shininess }, m_alpha{ l_alpha }
{
    m_diffusiveTexture = m_sceneManager->m_sharedContext->graphics->m_textures[l_texture].first;
    m_specularTexture  = m_sceneManager->m_sharedContext->graphics->m_textures[l_texture].second;
}

void ObjectWithTexture::Render()
{
    m_sceneManager->m_sharedContext->graphics->m_shader->SetDiffAndSpecTextures(m_diffusiveTexture, m_specularTexture);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("material.shininess", m_shininess);

    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("viewPosition",
                                                                      m_sceneManager->m_cameraPosition.x,
                                                                      m_sceneManager->m_cameraPosition.y,
                                                                      m_sceneManager->m_cameraPosition.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatMatrix("modelMatrix", m_modelMatrix.GetArray());
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("alpha", m_alpha);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatMatrix("transformMatrix", m_transformMatrix.GetArray());
}

void ObjectWithTexture::RenderPicking()
{
    m_sceneManager->m_sharedContext->graphics->m_pickingShader->SetFloatMatrix("transformMatrix", m_transformMatrix.GetArray());
    m_sceneManager->m_sharedContext->graphics->m_pickingShader->SetUInt("objectID", m_ID);
}

void ObjectWithTexture::Update()
{
    MatrixFloat m_translateMatrix;
    MatrixFloat m_rotateMatrix;
    MatrixFloat m_scaleMatrix;
    m_translateMatrix.Move(m_position.x, m_position.y, m_position.z);
    m_rotateMatrix.Rotate(m_rotation.x, m_rotation.y, m_rotation.z);
    m_scaleMatrix.ScaleXYZ(m_scale.x, m_scale.y, m_scale.z);
    m_modelMatrix     = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
    m_transformMatrix = m_sceneManager->m_projectionMatrix * m_sceneManager->m_viewMatrix * m_modelMatrix;
}

//* Cube

Cube::Cube(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
           Texture l_texture, GLuint l_shininess, GLfloat l_alpha)
    : ObjectWithTexture(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha) {}

Cube::~Cube() {}

void Cube::Render()
{
    ObjectWithTexture::Render();

    m_sceneManager->m_sharedContext->graphics->m_shader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawCube();
}

void Cube::RenderPicking()
{
    ObjectWithTexture::RenderPicking();

    m_sceneManager->m_sharedContext->graphics->m_pickingShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawCube();
}

void Cube::Update()
{
    ObjectWithTexture::Update();
}

//* Sphere

Sphere::Sphere(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
               Texture l_texture, GLuint l_shininess, GLfloat l_alpha)
    : ObjectWithTexture(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha) {}

Sphere::~Sphere() {}

void Sphere::Render()
{
    ObjectWithTexture::Render();

    m_sceneManager->m_sharedContext->graphics->m_shader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawSphere();
}

void Sphere::RenderPicking()
{
    ObjectWithTexture::RenderPicking();

    m_sceneManager->m_sharedContext->graphics->m_pickingShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawSphere();
}

void Sphere::Update()
{
    ObjectWithTexture::Update();
}

//* LightSource

LightSource::LightSource(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                         sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular)
    : Object{ l_ID, l_sceneManager, l_position, l_rotation, l_scale }, m_ambient{ l_ambient }, m_diffusive{ l_diffusive }, m_specular{ l_specular } {}

void LightSource::Update()
{
    MatrixFloat m_translateMatrix;
    MatrixFloat m_rotateMatrix;
    MatrixFloat m_scaleMatrix;
    m_translateMatrix.Move(m_position.x, m_position.y, m_position.z);
    m_rotateMatrix.Rotate(m_rotation.x, m_rotation.y, m_rotation.z);
    m_scaleMatrix.ScaleXYZ(m_scale.x, m_scale.y, m_scale.z);
    m_modelMatrix     = m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
    m_transformMatrix = m_sceneManager->m_projectionMatrix * m_sceneManager->m_viewMatrix * m_modelMatrix;
}

void LightSource::Render() {}

void LightSource::RenderPicking()
{
    // TODO make smol cube rendering
    m_sceneManager->m_sharedContext->graphics->m_pickingShader->SetFloatMatrix("transformMatrix", m_transformMatrix.GetArray());
    m_sceneManager->m_sharedContext->graphics->m_pickingShader->SetUInt("objectID", m_ID);
}

//* PointLight

PointLight::PointLight(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                       sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                       GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
    : LightSource{ l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular },
      m_constant{ l_constant }, m_linear{ l_linear }, m_quadratic{ l_quadratic } {}

PointLight::~PointLight() {}

void PointLight::Update()
{
    LightSource::Update();

    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].position", m_position.x, m_position.y, m_position.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].constant", m_constant);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].linear", m_linear);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].quadratic", m_quadratic);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].ambient", m_ambient.x, m_ambient.y, m_ambient.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].diffuse", m_diffusive.x, m_diffusive.y, m_diffusive.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("pointLights[" + std::to_string(m_sceneManager->m_pointLightCount) + "].specular", m_specular.x, m_specular.y, m_specular.z);

    m_sceneManager->m_pointLightCount++;
}

//* DirectionalLight

DirectionalLight::DirectionalLight(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                   sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular)
    : LightSource{ l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular } {}

DirectionalLight::~DirectionalLight() {}

void DirectionalLight::Update()
{
    LightSource::Update();

    sf::Vector3f direction;
    direction.x = std::cos(m_rotation.y) * std::cos(m_rotation.x);
    direction.y = std::sin(m_rotation.x);
    direction.z = std::sin(m_rotation.y) * std::cos(m_rotation.x);

    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("dirLight.direction", direction.x, direction.y, direction.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("dirLight.ambient", m_ambient.x, m_ambient.y, m_ambient.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("dirLight.diffuse", m_diffusive.x, m_diffusive.y, m_diffusive.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("dirLight.specular", m_specular.x, m_specular.y, m_specular.z);
}

//* SpotLight

SpotLight::SpotLight(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                     sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                     GLfloat l_cutOff, GLfloat l_outerCutOff, GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
    : LightSource{ l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular },
      m_cutOff{ l_cutOff }, m_outerCutOff{ l_outerCutOff }, m_constant{ l_constant }, m_linear{ l_linear }, m_quadratic{ l_quadratic } {}

SpotLight::~SpotLight() {}

void SpotLight::Update()
{
    LightSource::Update();

    sf::Vector3f direction;
    direction.x = std::cos(m_rotation.y) * std::cos(m_rotation.x);
    direction.y = std::sin(m_rotation.x);
    direction.z = std::sin(m_rotation.y) * std::cos(m_rotation.x);

    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].position", m_position.x, m_position.y, m_position.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].direction", direction.x, direction.y, direction.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].cutOff", m_cutOff);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].outerCutOff", m_outerCutOff);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].constant", m_constant);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].linear", m_linear);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloat("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].quadratic", m_quadratic);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].ambient", m_ambient.x, m_ambient.y, m_ambient.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].diffuse", m_diffusive.x, m_diffusive.y, m_diffusive.z);
    m_sceneManager->m_sharedContext->graphics->m_shader->SetFloatVec3("spotLight[" + std::to_string(m_sceneManager->m_spotLightCount) + "].specular", m_specular.x, m_specular.y, m_specular.z);

    m_sceneManager->m_spotLightCount++;
}

//* LightCube

LightCube::LightCube(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation,
                     sf::Vector3f l_scale, sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                     GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
    : PointLight(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_constant, l_linear, l_quadratic) {}

LightCube::~LightCube() {}

void LightCube::Render()
{
    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->SetFloatMatrix("transformMatrix", m_transformMatrix.GetArray());
    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->SetFloatVec3("lightColor", m_specular.x, m_specular.y, m_specular.z);

    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawLightSourceCube();
}

void LightCube::RenderPicking()
{
    LightSource::RenderPicking();

    m_sceneManager->m_sharedContext->graphics->m_pickingShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawCube();
}

void LightCube::Update()
{
    PointLight::Update();
}

//* LightSphere

LightSphere::LightSphere(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation,
                         sf::Vector3f l_scale, sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                         GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
    : PointLight(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_constant, l_linear, l_quadratic) {}

LightSphere::~LightSphere() {}

void LightSphere::Render()
{
    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->SetFloatMatrix("transformMatrix", m_transformMatrix.GetArray());
    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->SetFloatVec3("lightColor", m_specular.x, m_specular.y, m_specular.z);

    m_sceneManager->m_sharedContext->graphics->m_lightSourceShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawLightSourceSphere();
}

void LightSphere::RenderPicking()
{
    LightSource::RenderPicking();

    m_sceneManager->m_sharedContext->graphics->m_pickingShader->Use();
    m_sceneManager->m_sharedContext->graphics->DrawLightSourceSphere();
}

void LightSphere::Update()
{
    PointLight::Update();
}

//* ObjectWithLight

ObjectWithLight::ObjectWithLight(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                 Texture l_texture, GLuint l_shininess, GLfloat l_alpha, std::vector<LightSource*> l_lights)
    : ObjectWithTexture(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha), m_lights{ l_lights } {}

ObjectWithLight::~ObjectWithLight()
{
    for (auto lightSource : m_lights)
    {
        delete lightSource;
    }
}

void ObjectWithLight::Render()
{
}

void ObjectWithLight::RenderPicking()
{
}

void ObjectWithLight::Update()
{
}

//* Lamp

Lamp::Lamp(GLint l_ID, SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
           Texture l_texture, GLuint l_shininess, GLfloat l_alpha, std::vector<LightSource*> l_lights)
    : ObjectWithLight(l_ID, l_sceneManager, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha, l_lights) {}

Lamp::~Lamp()
{
}

void Lamp::Render()
{
}

void Lamp::RenderPicking()
{
}

void Lamp::Update()
{
}

//* SceneManager

SceneManager::SceneManager(SharedContext* l_sharedContext) : m_sharedContext{ l_sharedContext } {}

SceneManager::~SceneManager()
{
    for (const auto elem : m_objects)
    {
        delete elem;
    }
    for (const auto elem : m_lightSources)
    {
        delete elem;
    }
}

Cube* SceneManager::MakeCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                             Texture l_texture, GLuint l_shininess, GLfloat l_alpha)
{
    Cube* cube = new Cube(++m_objectCount, this, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha);
    m_objects.push_back(cube);
    return cube;
}

Sphere* SceneManager::MakeSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, Texture l_texture, GLuint l_shininess, GLfloat l_alpha)
{
    Sphere* sphere = new Sphere(++m_objectCount, this, l_position, l_rotation, l_scale, l_texture, l_shininess, l_alpha);
    m_objects.push_back(sphere);
    return sphere;
}

PointLight* SceneManager::MakePointLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular, GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
{
    PointLight* pointLight = new PointLight(++m_objectCount, this, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_constant, l_linear, l_quadratic);
    m_lightSources.push_back(pointLight);
    return pointLight;
}

LightCube* SceneManager::MakeLightCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                       sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                                       GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
{
    LightCube* lightCube = new LightCube(++m_objectCount, this, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_constant, l_linear, l_quadratic);
    m_lightSources.push_back(lightCube);
    return lightCube;
}

LightSphere* SceneManager::MakeLightSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                           sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                                           GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
{
    LightSphere* lightSphere = new LightSphere(++m_objectCount, this, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_constant, l_linear, l_quadratic);
    m_lightSources.push_back(lightSphere);
    return lightSphere;
}

DirectionalLight* SceneManager::MakeDirectionalLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular)
{
    DirectionalLight* dirLight = new DirectionalLight(++m_objectCount, this, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular);
    m_lightSources.push_back(dirLight);
    return dirLight;
}

SpotLight* SceneManager::MakeSpotLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale, sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular, GLfloat l_cutOff, GLfloat l_outerCutOff, GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic)
{
    SpotLight* spotLight = new SpotLight(++m_objectCount, this, l_position, l_rotation, l_scale, l_ambient, l_diffusive, l_specular, l_cutOff, l_outerCutOff, l_constant, l_linear, l_quadratic);
    m_lightSources.push_back(spotLight);
    return spotLight;
}

void SceneManager::HandleInput()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*m_sharedContext->window->GetWindow());

        unsigned int windowWidth  = m_sharedContext->window->GetWindowSize().x;
        unsigned int windowHeight = m_sharedContext->window->GetWindowSize().y;

        GLuint pixelX = (GLuint)(((float)mousePos.x / windowWidth) * m_sharedContext->graphics->m_pickingWidth);
        GLuint pixelY = (GLuint)(((float)(windowHeight - 1 - mousePos.y) / windowHeight) * m_sharedContext->graphics->m_pickingHeight);

        sf::Vector2f NDC_for_GUI_check = m_sharedContext->GUI->ConvertScreenCoordinates(sf::Mouse::getPosition(*m_sharedContext->window->GetWindow())); // Используем SFML координаты здесь, если GUI ожидает их
        if (NDC_for_GUI_check.x < m_sharedContext->GUI->m_leftBorder ||
            NDC_for_GUI_check.x > m_sharedContext->GUI->m_rightBorder ||
            NDC_for_GUI_check.y < m_sharedContext->GUI->m_elements.back()->m_topBorder - m_sharedContext->GUI->m_elementHeight ||
            NDC_for_GUI_check.y > m_sharedContext->GUI->m_elements.front()->m_topBorder)
        {
            m_pickingCords.x = pixelX;
            m_pickingCords.y = pixelY;
            m_flagPicked     = true;
            // std::cout << pixelX << " " << pixelY << '\n';
        }
    }
}

void SceneManager::Update()
{
    m_cameraPosition = m_sharedContext->camera->getPosition();

    m_viewMatrix = m_sharedContext->camera->getViewMatrix();

    if (m_flagProjection)
    {
        m_projectionMatrix.OrthographicProjection((float)m_sharedContext->window->GetWindowSize().x /
                                                      (float)m_sharedContext->window->GetWindowSize().y,
                                                  1.0f, -1.0f, 1.0f, -1.0f, m_nearDistance, 100.0f);
    }
    else
    {
        m_projectionMatrix.PerspectiveProjection(m_sharedContext->camera->getFOV(),
                                                 (float)m_sharedContext->window->GetWindowSize().x /
                                                     (float)m_sharedContext->window->GetWindowSize().y,
                                                 m_nearDistance, 100.0f);
    }

    for (const auto elem : m_objects)
    {
        elem->Update();
    }

    m_spotLightCount  = 0;
    m_pointLightCount = 0;
    for (const auto elem : m_lightSources)
    {
        elem->Update();
    }

    m_sharedContext->graphics->m_shader->SetInt("pointLightCount", m_pointLightCount);
    m_sharedContext->graphics->m_shader->SetInt("spotLightCount", m_spotLightCount);

    if (m_flagPickedReady == true)
    {
        m_pickedObject = m_sharedContext->graphics->GetPickingResult();
        if (m_pickedObject != 0)
        {
            std::cout << m_pickedObject << '\n';
        }
        m_flagPickedReady = false;
    }
}

void SceneManager::Render()
{
    if (m_flagPicked == true)
    {
        RenderPicking();

        m_sharedContext->graphics->ReadPixel(m_pickingCords.x, m_pickingCords.y);

        m_flagPicked      = false;
        m_flagPickedReady = true;
    }

    for (const auto elem : m_lightSources)
    {
        elem->Render();
    }

    for (const auto elem : m_objects)
    {
        elem->Render();
    }
}

void SceneManager::RenderPicking()
{
    m_sharedContext->graphics->BeginPickingDraw();

    for (const auto elem : m_lightSources)
    {
        elem->RenderPicking();
    }

    for (const auto elem : m_objects)
    {
        elem->RenderPicking();
    }

    m_sharedContext->graphics->EndPickingDraw(m_sharedContext->window->GetWindowSize().x, m_sharedContext->window->GetWindowSize().y);
}
