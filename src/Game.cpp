#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(800, 600)), m_GUI(&m_sharedContext, 0.6f, 1.0f, 0.1f, 0.045f),
               m_sceneManager(&m_sharedContext)
{
    m_sharedContext.graphics      = &m_graphics;
    m_sharedContext.window        = &m_window;
    m_sharedContext.GUI           = &m_GUI;
    m_sharedContext.shader        = &m_graphics.m_shader;
    m_sharedContext.textShader    = &m_graphics.m_textShader;
    m_sharedContext.GUIShader     = &m_graphics.m_GUIShader;
    m_sharedContext.pickingShader = &m_graphics.m_pickingShader;
    m_camera                      = m_sharedContext.camera;

    Material diamond{
        m_graphics.m_textures[0].diffuse, m_graphics.m_textures[0].specular, 64, 1.0f, { 1.0f, 1.0f, 1.0f },
            true, true
    };

    Material debugMat{
        0, 0, 64, 1.0f, { 1.0f, 0.0f, 0.0f },
            false, false
    };

    LightComponent pointLight{
        LightType::Point, { 0.01f, 0.01f, 0.01f },
         {  0.5f,  0.5f,  0.5f },
         {  1.0f,  1.0f,  1.0f }
    };

    LightComponent dirLight{
        LightType::Directional, { 0.2f, 0.2f, 0.2f },
         {  0.8, 0.8f, 0.8f },
         { 0.8f, 0.8f, 0.8f }
    };
    dirLight.direction = { 0.0f, -1.0f, 0.0f };

    m_cube = m_sceneManager.MakeCube({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }, debugMat);
    m_sceneManager.MakeCube({ 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 1.8f, 0.8f, 0.8f }, diamond);
    m_lightCube = m_sceneManager.MakeLightCube({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.2f, 0.2f }, pointLight);
    // m_sceneManager.MakeLightSphere({ 1.0, 2.0, 2.0f }, { 0.0, 20.0, 20.0 }, { 0.1, 0.1, 0.1 }, pointLight);
    m_sceneManager.MakeSphere({ 2.0f, 1.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.1f, 0.5f, 0.5f }, diamond);
    m_sceneManager.MakeSphere({ 0.0f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.5f, 0.2f }, diamond);
    m_sceneManager.MakeCube({ 0.0f, -2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 100.0f, 0.1f, 100.0f }, diamond);
    m_sceneManager.MakeDirectionalLight({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, dirLight);
    // m_sceneManager.MakeLightSphere({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.2f, { 1.0f, 1.0f, 1.0f });

    m_GUI.MakeSlider("Rotate angle", &m_rotateAngle, -1.0, 1.0);
    m_GUI.MakeButton("Projection switch", std::bind([this]()
                                                    { m_sceneManager.SetProjectionFlag(!m_sceneManager.IsProjectionOrthographic()); }));
    // m_GUI.MakeSlider("Element gap", &(m_GUI.m_elementGap), 0, 0.11);
    // m_GUI.MakeSlider("Element height", &(m_GUI.m_elementHeight), 0.15, 0.30);
    // m_GUI.MakeSlider("Left border", &(m_GUI.m_leftBorder), -1, 0.85);
    m_GUI.MakeHUDElement("Time", std::bind([this]()
                                           { return m_elapsedFixed.asSeconds(); }));
    m_GUI.MakeHUDElement("Time for tick", std::bind([this]()
                                                    { return m_elapsed.asSeconds(); }));
    m_GUI.MakeHUDElement("Object count", std::bind([this]()
                                                   { return std::to_string(m_sceneManager.GetObjectCount()); }));
    m_GUI.MakeButton("Rotate switch", std::bind([this]()
                                                { m_flagRotate = !m_flagRotate; }));
    // m_GUI.MakeSlider("Light red value", &m_lightCube->m_diffusive.x, 0, 1);
    // m_GUI.MakeSlider("Light green value", &m_lightCube->m_diffusive.y, 0, 1);
    // m_GUI.MakeSlider("Light blue value", &m_lightCube->m_diffusive.z, 0, 1);
    m_GUI.MakeButton("Switch following", std::bind([this]()
                                                   { m_flagFollow = !m_flagFollow; }));
    m_GUI.MakeSlider("Radius", &m_radius, 0, 10);
    m_GUI.MakeSlider("Camera distance", &m_cameraDistance, 0, 20);
    m_GUI.MakeHUDElement("Picked object", std::bind([this]()
                                                    { return std::to_string(m_sceneManager.GetPickedObjectId()); }));

    // m_GUI.MakeSlider("Near distance", &m_sceneManager.m_nearDistance, 0.1, 10);
    m_GUI.MakeButton("Spawn lightCube", std::bind([this]()
                                                  { 
            m_targetPoint = m_camera->GetTargetPoint(m_cameraDistance);
            LightComponent pointLight{
                LightType::Point, { 0.2f, 0.2f, 0.2f },
                { 0.8f, 0.8f, 0.8f },
                { 1.0f, 1.0f, 1.0f }
            };;
            m_sceneManager.MakeLightCube(m_targetPoint, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f }, pointLight); }));

    m_GUI.MakeButton("Toggle YawGlobal", std::bind([this]()
                                                   { m_camera->ToggleYawGlobal(); }));
    m_GUI.MakeHUDElement("Camera pitch", std::bind([this]()
                                                   { return std::to_string(m_camera->GetRotation().x); }));
    m_GUI.MakeHUDElement("Camera yaw", std::bind([this]()
                                                 { return std::to_string(m_camera->GetRotation().y); }));
    m_GUI.MakeHUDElement("Camera roll", std::bind([this]()
                                                  { return std::to_string(m_camera->GetRotation().z); }));
}

Game::~Game() {}

void Game::Update()
{
    m_window.Update();

    m_cube->Rotate({ (float)m_rotateAngle, 0, 0 });

    if (m_flagRotate)
    {
        if (m_flagFollow)
        {
            m_targetPoint = m_camera->GetTargetPoint(m_cameraDistance);
        }
        m_cube->SetPosition({ m_targetPoint.x,
                              m_targetPoint.y + std::sin(m_cube->GetRotation().x * DEG2RAD) * (float)m_radius,
                              m_targetPoint.z + std::cos(m_cube->GetRotation().x * DEG2RAD) * (float)m_radius });
    }

    m_sceneManager.Update(m_elapsed.asMilliseconds());

    m_GUI.Update(m_elapsed.asMilliseconds());
}

void Game::Render()
{
    m_window.BeginDraw();

    m_sceneManager.Render();

    m_GUI.Render();

    m_window.EndDraw();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::HandleInput()
{
    m_sceneManager.HandleInput();

    m_GUI.HandleInput();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();

    m_elapsedFixed += m_elapsed;
}