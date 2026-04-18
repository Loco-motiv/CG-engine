#include "SceneManager.h"

SceneManager::SceneManager(SharedContext* l_sharedContext)
    : m_sharedContext{ l_sharedContext },
      m_camera(-1, 30.0f, 0.05f, 0.3f, { 0.0f, 0.0f, 5.0f }) {}

SceneManager::~SceneManager()
{
}

void SceneManager::LoadScene(const fs::path& l_scenePath)
{
    std::ifstream file(l_scenePath);
    if (file.is_open())
    {
        file >> m_camera;
        file >> m_nearDistance >> m_rearDistance >> m_pickedObject;
        GLuint ID;
        Transform transform;
        std::string meshName, materialName, shaderName;
        LightComponent light;
        while (file >> ID >> transform >> meshName >> materialName >> shaderName >> light)
        {
            std::cout << ID << " " << transform << " " << meshName << " " << materialName << ' ' << shaderName << ' ' << light << '\n';

            MakeObject(ID, transform, meshName, materialName, light, shaderName);
        }
    }
}

void SceneManager::SaveScene(const fs::path& l_scenePath, std::string_view l_name)
{
    const auto timeNow    = std::chrono::system_clock::now();
    const auto localTime  = std::chrono::current_zone()->to_local(timeNow);
    std::string timestamp = std::format("{:%d-%m-%Y_%H-%M-%S}", localTime);
    std::string name      = std::string(l_name) + "_" + timestamp + ".txt";
    std::ofstream file(l_scenePath / name);

    if (file.is_open())
    {
        file << m_camera << '\n';
        file << m_nearDistance << ' ' << m_rearDistance << ' ' << m_pickedObject << '\n';
        for (const auto& object : m_objects)
        {
            file << *object << '\n';
        }
    }
}

Object* SceneManager::MakeCube(Transform l_transform, std::string l_materialName)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* cube = m_objects.back().get();
    cube->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    cube->SetMesh(m_sharedContext->meshManager->Get("cube.obj"));
    cube->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return cube;
}

Object* SceneManager::MakeSphere(Transform l_transform, std::string l_materialName)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* sphere = m_objects.back().get();
    sphere->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    sphere->SetMesh(m_sharedContext->meshManager->Get("sphere.obj"));
    sphere->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return sphere;
}

Object* SceneManager::MakePointLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* pointLight = m_objects.back().get();
    pointLight->SetLight(l_light);
    pointLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return pointLight;
}

Object* SceneManager::MakeLightCube(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* lightCube = m_objects.back().get();
    lightCube->SetLight(l_light);
    lightCube->SetMesh(m_sharedContext->meshManager->Get("cube.obj"));
    lightCube->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    lightCube->SetMaterial(m_sharedContext->materialManager->Get("LightSource.txt"));
    return lightCube;
}

Object* SceneManager::MakeLightSphere(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* lightSphere = m_objects.back().get();
    lightSphere->SetLight(l_light);
    lightSphere->SetMesh(m_sharedContext->meshManager->Get("sphere.obj"));
    lightSphere->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    lightSphere->SetMaterial(m_sharedContext->materialManager->Get("LightSource.txt"));
    return lightSphere;
}

Object* SceneManager::MakeDirectionalLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* dirLight = m_objects.back().get();
    dirLight->SetLight(l_light);
    dirLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return dirLight;
}

Object* SceneManager::MakeSpotLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* spotLight = m_objects.back().get();
    spotLight->SetLight(l_light);
    spotLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return spotLight;
}

Object* SceneManager::MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* object = m_objects.back().get();
    object->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    object->SetMesh(m_sharedContext->meshManager->Get(l_meshName));
    object->SetLight(l_light);
    object->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return object;
}

Object* SceneManager::MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light, std::string l_shaderName)
{
    m_objects.push_back(std::make_unique<Object>(++m_objectCount, l_transform));
    Object* object = m_objects.back().get();
    object->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    object->SetMesh(m_sharedContext->meshManager->Get(l_meshName));
    object->SetLight(l_light);
    object->SetShader(m_sharedContext->shaderManager->Get(l_shaderName));
    return object;
}

Object* SceneManager::MakeObject(GLuint l_ID, Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light, std::string l_shaderName)
{
    m_objects.push_back(std::make_unique<Object>(l_ID, l_transform));
    m_objectCount++;
    Object* object = m_objects.back().get();
    object->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    object->SetMesh(m_sharedContext->meshManager->Get(l_meshName));
    object->SetLight(l_light);
    object->SetShader(m_sharedContext->shaderManager->Get(l_shaderName));
    return object;
}

void SceneManager::HandleInput()
{
    if (m_sharedContext->inputManager->IsGUITookInput() == false)
    {
        if (m_sharedContext->inputManager->IsMouseButtonReleased(sf::Mouse::Left))
        {
            sf::Vector2i mousePos = m_sharedContext->inputManager->GetMousePosition();

            GLuint windowWidth  = m_sharedContext->window->GetWindowSize().x;
            GLuint windowHeight = m_sharedContext->window->GetWindowSize().y;

            GLuint pixelX = (GLuint)(((float)mousePos.x / windowWidth) * m_sharedContext->graphics->m_pickingWidth);
            GLuint pixelY = (GLuint)(((float)(windowHeight - 1 - mousePos.y) / windowHeight) * m_sharedContext->graphics->m_pickingHeight);

            m_pickingCords.x = pixelX;
            m_pickingCords.y = pixelY;
            m_flagPicked     = true;
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

        if (m_sharedContext->inputManager->IsKeyReleased(sf::Keyboard::M))
        {
            m_flagReleaseMouse = !m_flagReleaseMouse;
            (*m_sharedContext->window->GetWindow()).setMouseCursorVisible(m_flagReleaseMouse);
            sf::Vector2i windowCenter = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2,
                                                     m_sharedContext->window->GetWindowSize().y / 2);
            sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
        }
        if (m_sharedContext->inputManager->IsKeyReleased(sf::Keyboard::I))
        {
            SaveScene("scenes/");
        }

        m_camera.HandleInput(xDelta, yDelta);
    }
}

void SceneManager::Update(GLint l_elapsed)
{
    if (m_flagPickedReady == true)
    {
        m_pickedObject    = m_sharedContext->graphics->GetPickingResult();
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

    for (auto& elem : m_objects)
    {
        elem->UpdateTransformMatrix(projectionViewMatrix);

        LightType elemType = elem->GetLight().type;

        if (elemType == LightType::Point)
        {
            m_pointLightCount++;
        }
        else if (elemType == LightType::Spot)
        {
            m_spotLightCount++;
        }
        if (elemType != LightType::None and elem->GetLight().hasChanges)
        {
            m_sharedContext->renderer->UpdateLightData(elem.get(), m_pointLightCount, m_spotLightCount);
        }
    }

    // TODO maybe i should do these computations in update...

    if (m_flagPicked == true)
    {
        m_sharedContext->renderer->RenderObjectsPicking(m_objects);

        m_sharedContext->renderer->ReadPickingPixel(m_pickingCords.x, m_pickingCords.y);

        m_flagPicked      = false;
        m_flagPickedReady = true;
    }

    m_sharedContext->renderer->RenderObjects(m_objects, m_cameraPosition);
}