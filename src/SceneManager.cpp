#include "SceneManager.h"

#define TINYBVH_IMPLEMENTATION
#include "tiny_bvh.h"

SceneManager::SceneManager(SharedContext* l_sharedContext)
    : m_sharedContext{ l_sharedContext },
      m_camera(-1, 90.0f, 10.0f, 0.3f, { 0.0f, 0.0f, 5.0f }) {}

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
        GLint ID;
        Transform transform;
        std::string meshName, materialName, shaderName;
        LightComponent light;
        while (file >> ID >> transform >> meshName >> materialName >> shaderName >> light)
        {
            MakeObject(ID, transform, meshName, materialName, light, shaderName);
            m_lastID = std::max(m_lastID, ID);
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
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* cube = m_objects.back().get();
    cube->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    cube->SetMesh(m_sharedContext->meshManager->Get("cube.obj:pCube1"));
    cube->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return cube;
}

Object* SceneManager::MakeSphere(Transform l_transform, std::string l_materialName)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* sphere = m_objects.back().get();
    sphere->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    sphere->SetMesh(m_sharedContext->meshManager->Get("sphere.obj:pSphere1"));
    sphere->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return sphere;
}

Object* SceneManager::MakePointLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* pointLight = m_objects.back().get();
    pointLight->SetLight(l_light);
    pointLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return pointLight;
}

Object* SceneManager::MakeLightCube(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* lightCube = m_objects.back().get();
    lightCube->SetLight(l_light);
    lightCube->SetMesh(m_sharedContext->meshManager->Get("cube.obj:pCube1"));
    lightCube->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    lightCube->SetMaterial(m_sharedContext->materialManager->Get("LightSource.mtl:material0"));
    return lightCube;
}

Object* SceneManager::MakeLightSphere(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* lightSphere = m_objects.back().get();
    lightSphere->SetLight(l_light);
    lightSphere->SetMesh(m_sharedContext->meshManager->Get("sphere.obj:pSphere1"));
    lightSphere->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    lightSphere->SetMaterial(m_sharedContext->materialManager->Get("LightSource.mtl:material0"));
    return lightSphere;
}

Object* SceneManager::MakeDirectionalLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* dirLight = m_objects.back().get();
    dirLight->SetLight(l_light);
    dirLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return dirLight;
}

Object* SceneManager::MakeSpotLight(Transform l_transform, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* spotLight = m_objects.back().get();
    spotLight->SetLight(l_light);
    spotLight->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return spotLight;
}

Object* SceneManager::MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
    Object* object = m_objects.back().get();
    object->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    object->SetMesh(m_sharedContext->meshManager->Get(l_meshName));
    object->SetLight(l_light);
    object->SetShader(m_sharedContext->shaderManager->Get("Main.txt"));
    return object;
}

Object* SceneManager::MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light, std::string l_shaderName)
{
    m_objects.push_back(std::make_unique<Object>(++m_lastID, l_transform));
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
    m_lastID++;
    Object* object = m_objects.back().get();
    object->SetMaterial(m_sharedContext->materialManager->Get(l_materialName));
    object->SetMesh(m_sharedContext->meshManager->Get(l_meshName));
    object->SetLight(l_light);
    object->SetShader(m_sharedContext->shaderManager->Get(l_shaderName));
    return object;
}

void SceneManager::MakeObjectsFromModel(const fs::path& l_modelPath, const fs::path& l_matPath)
{
    std::vector<std::pair<std::string, std::string>> meshAndMaterialNames = m_sharedContext->graphics->GetMeshAndMaterialNames(l_modelPath.string(), "resources/materials/");
    for (const auto& [meshName, materialName] : meshAndMaterialNames)
    {
        MakeObject({}, l_modelPath.filename().string() + ":" + meshName, l_matPath.filename().string() + ":" + materialName, LightComponent());
    }
}

void SceneManager::BuildBVHAndSendToGPU()
{
    std::vector<tinybvh::bvhvec4> m_bvhVertices;
    std::vector<VertexGPU> m_vertices;
    std::vector<uint32_t> m_triMaterials;
    std::vector<MaterialGPU> m_gpuMaterials;
    tinybvh::BVH m_bvh = tinybvh::BVH();
    std::unordered_map<const Material*, uint32_t> materialIndexMap;
    for (const auto& obj : m_objects)
    {
        Mesh* mesh         = obj->GetMesh();
        Material* material = obj->GetMaterial();
        if (!mesh or !material)
        {
            continue;
        }
        uint32_t matIdx;
        auto it = materialIndexMap.find(material);
        if (it != materialIndexMap.end())
        {
            matIdx = it->second;
        }
        else
        {
            MaterialGPU gpuMat;
            gpuMat.diffColour  = material->diffuseColour;
            gpuMat.alpha       = material->alpha;
            gpuMat.specColour  = material->specularColour;
            gpuMat.shininess   = material->shininess;
            gpuMat.diffuseTex  = material->diffuseTexture ? material->diffuseTexture->handle : 0;
            gpuMat.specularTex = material->specularTexture ? material->specularTexture->handle : 0;
            gpuMat.alphaTex    = material->alphaTexture ? material->alphaTexture->handle : 0;
            gpuMat.normalTex   = material->normalTexture ? material->normalTexture->handle : 0;
            uint32_t idx       = static_cast<uint32_t>(m_gpuMaterials.size());
            m_gpuMaterials.push_back(gpuMat);
            materialIndexMap[material] = idx;
            matIdx                     = idx;
        }
        MatrixFloat modelMatrix  = obj->GetModelMatrix();
        MatrixFloat normalMatrix = modelMatrix.Inverse().Transpose();
        for (size_t i = 0; i < mesh->m_indices.size(); i += 3)
        {
            m_triMaterials.push_back(matIdx);
            for (int j = 0; j < 3; ++j)
            {
                Vertex& v                 = mesh->m_vertices[mesh->m_indices[i + j]];
                sf::Vector3f worldPos     = modelMatrix * sf::Vector3f(v.px, v.py, v.pz);
                sf::Vector3f worldNormal  = normalMatrix * sf::Vector3f(v.nx, v.ny, v.nz);
                sf::Vector3f worldTangent = normalMatrix * sf::Vector3f(v.tx, v.ty, v.tz);
                worldNormal               = worldNormal.normalized();
                worldTangent              = worldTangent.normalized();

                VertexGPU gpuVert;
                gpuVert.px  = worldPos.x;
                gpuVert.py  = worldPos.y;
                gpuVert.pz  = worldPos.z;
                gpuVert.u   = v.u;
                gpuVert.nx  = worldNormal.x;
                gpuVert.ny  = worldNormal.y;
                gpuVert.nz  = worldNormal.z;
                gpuVert.v   = v.v;
                gpuVert.tx  = worldTangent.x;
                gpuVert.ty  = worldTangent.y;
                gpuVert.tz  = worldTangent.z;
                gpuVert.pad = 0.0f;
                m_vertices.push_back(gpuVert);
                m_bvhVertices.push_back({ worldPos.x, worldPos.y, worldPos.z, 0.0f });
            }
        }
    }
    if (m_bvhVertices.empty())
    {
        return;
    }
    m_bvh.BuildHQ(m_bvhVertices.data(), m_bvhVertices.size() / 3);
    GLuint triCount = m_bvh.idxCount;

    std::vector<VertexGPU> sortedVertices(triCount * 3);
    std::vector<uint32_t> sortedTriMaterials(triCount);
    for (uint32_t i = 0; i < triCount; ++i)
    {
        uint32_t originalTriIdx   = m_bvh.primIdx[i];
        sortedTriMaterials[i]     = m_triMaterials[originalTriIdx];
        sortedVertices[i * 3 + 0] = m_vertices[originalTriIdx * 3 + 0];
        sortedVertices[i * 3 + 1] = m_vertices[originalTriIdx * 3 + 1];
        sortedVertices[i * 3 + 2] = m_vertices[originalTriIdx * 3 + 2];
    }
    m_sharedContext->graphics->SetSSBOData(1, m_bvh.usedNodes * sizeof(tinybvh::BVH::BVHNode), m_bvh.bvhNode, GL_DYNAMIC_DRAW);
    m_sharedContext->graphics->SetSSBOData(2, sortedVertices.size() * sizeof(VertexGPU), sortedVertices.data(), GL_DYNAMIC_DRAW);
    m_sharedContext->graphics->SetSSBOData(3, sortedTriMaterials.size() * sizeof(uint32_t), sortedTriMaterials.data(), GL_DYNAMIC_DRAW);
    m_sharedContext->graphics->SetSSBOData(4, m_gpuMaterials.size() * sizeof(MaterialGPU), m_gpuMaterials.data(), GL_DYNAMIC_DRAW);

    m_flagBVHReady = true;
}

void SceneManager::DeleteObject(GLint l_id)
{
    if (l_id == m_pickedObject)
    {
        m_pickedObject = 0;
    }
    std::erase_if(m_objects, [l_id](const std::unique_ptr<Object>& obj)
                  { return obj->GetID() == l_id; });
}

void SceneManager::ClearScene()
{
    m_objects.clear();
    m_pickedObject = 0;
    m_lastID       = 0;
}

void SceneManager::HandleInput()
{
    if (m_sharedContext->inputManager->IsGUITookInput() == false)
    {
        if (m_sharedContext->inputManager->IsMouseButtonReleased(sf::Mouse::Button::Left))
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

        if (m_sharedContext->inputManager->IsKeyReleased(sf::Keyboard::Key::M))
        {
            m_flagReleaseMouse = !m_flagReleaseMouse;
            (*m_sharedContext->window->GetWindow()).setMouseCursorVisible(m_flagReleaseMouse);
            sf::Vector2i windowCenter = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2,
                                                     m_sharedContext->window->GetWindowSize().y / 2);
            sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
        }
        if (m_sharedContext->inputManager->IsKeyReleased(sf::Keyboard::Key::I))
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
        GLint pickResult = m_sharedContext->graphics->GetPickingResult();
        if (pickResult != -1)
        {
            m_pickedObject    = pickResult;
            m_flagPickedReady = false;
        }
    }

    m_camera.Update(l_elapsed);
}

void SceneManager::Render()
{
    sf::Vector3f m_cameraPosition = m_camera.GetPosition();

    m_viewMatrix = m_camera.GetViewMatrix();

    if (m_renderMode == RenderMode::Rasterization)
    {
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

        MatrixFloat projectionViewMatrix = m_projectionMatrix * m_viewMatrix;

        for (auto& elem : m_objects)
        {
            elem->UpdateTransformMatrix(projectionViewMatrix);
        }

        m_sharedContext->renderer->UpdateLightData(m_objects);

        m_sharedContext->renderer->RenderObjects(m_objects, m_cameraPosition);
    }
    else
    {
        switch (m_renderMode)
        {
        case RenderMode::PathTracing:
        {
            Shader* shader = m_sharedContext->renderer->SetPTShader("PT.txt");
            shader->SetBool("enableFrameMix", m_flagFrameAccumulation);
            break;
        }
        case RenderMode::ReSTIRGI:
        {
            Shader* shader = m_sharedContext->renderer->SetPTShader("ReSTIR.txt");
            shader->SetBool("enableTemporalReuse", true);
            shader->SetBool("enableSpatialReuse", true);
            shader->SetBool("enableFrameMix", m_flagFrameAccumulation);
            break;
        }
        case RenderMode::ReSTIRGItemporal:
        {
            Shader* shader = m_sharedContext->renderer->SetPTShader("ReSTIR.txt");
            shader->SetBool("enableTemporalReuse", true);
            shader->SetBool("enableSpatialReuse", false);
            shader->SetBool("enableFrameMix", m_flagFrameAccumulation);
            break;
        }
        case RenderMode::ReSTIRGIspacial:
        {
            Shader* shader = m_sharedContext->renderer->SetPTShader("ReSTIR.txt");
            shader->SetBool("enableTemporalReuse", false);
            shader->SetBool("enableSpatialReuse", true);
            shader->SetBool("enableFrameMix", m_flagFrameAccumulation);
            break;
        }
        default:
            break;
        }
        if (m_sceneChanged)
        {
            BuildBVHAndSendToGPU();
            m_sceneChanged = false;
        }
        if (m_flagBVHReady == false)
        {
            return;
        }
        sf::Vector3f forward = m_camera.GetForward();
        sf::Vector3f right   = m_camera.GetRight();
        sf::Vector3f up      = m_camera.GetUp();
        GLfloat fov          = m_camera.GetFOV();
        if (m_camera.HasChanges())
        {
            m_stillFramesCounter = 0;
        }
        else
        {
            m_stillFramesCounter++;
        }
        m_sharedContext->renderer->RenderObjectsPT(m_cameraPosition, fov, forward, right, up, m_stillFramesCounter);
    }

    if (m_flagPicked == true)
    {
        if (m_renderMode != RenderMode::Rasterization)
        {
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

            MatrixFloat projectionViewMatrix = m_projectionMatrix * m_viewMatrix;

            for (auto& elem : m_objects)
            {
                elem->UpdateTransformMatrix(projectionViewMatrix);
            }
        }
        m_sharedContext->renderer->RenderObjectsPicking(m_objects);

        m_sharedContext->renderer->ReadPickingPixel(m_pickingCords.x, m_pickingCords.y);

        m_flagPicked      = false;
        m_flagPickedReady = true;
    }
}