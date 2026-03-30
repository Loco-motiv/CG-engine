#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(800, 600)), m_graphics(),
               m_textureManager(&m_sharedContext, "resources/textures/"), m_materialManager(&m_sharedContext, "resources/materials/"),
               m_meshManager(&m_sharedContext, "resources/meshes/"), m_shaderManager(&m_sharedContext, "resources/shaderPrograms/"),
               m_renderer(&m_sharedContext), m_GUI(&m_sharedContext, 0.6f, 1.0f, 0.1f, 0.045f), m_sceneManager(&m_sharedContext)
{
    m_sharedContext.graphics        = &m_graphics;
    m_sharedContext.renderer        = &m_renderer;
    m_sharedContext.window          = &m_window;
    m_sharedContext.GUI             = &m_GUI;
    m_sharedContext.textureManager  = &m_textureManager;
    m_sharedContext.materialManager = &m_materialManager;
    m_sharedContext.meshManager     = &m_meshManager;
    m_sharedContext.shaderManager   = &m_shaderManager;

    m_sceneManager.LoadScene("scenes/Scene_30-03-2026_19-00-54.327957800.txt");
    m_camera = m_sceneManager.GetCamera();
    m_cube   = m_sceneManager.GetObject(1);

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
            LightComponent pointLight = LightComponent::Point(
                { 0.2f, 0.2f, 0.2f },
                { 0.8f, 0.8f, 0.8f },
                { 1.0f, 1.0f, 1.0f });
            m_sceneManager.MakeLightCube({m_targetPoint, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f }}, pointLight); }));

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
                              m_targetPoint.y + std::cos(m_cube->GetRotation().x * DEG2RAD) * (float)m_radius,
                              m_targetPoint.z + std::sin(m_cube->GetRotation().x * DEG2RAD) * (float)m_radius });
    }

    m_sceneManager.Update(m_elapsed.asMilliseconds());

    m_GUI.Update(m_elapsed.asMilliseconds());
}

void Game::Render()
{
    m_window.BeginDraw();

    m_sceneManager.Render();

    // m_GUI.Render(); //TODO make it work

    m_window.EndDraw();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::HandleInput()
{
    // save scene on i
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
    {
        m_sceneManager.SaveScene("scenes/");
    }
    m_sceneManager.HandleInput();

    m_GUI.HandleInput();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();

    m_elapsedFixed += m_elapsed;
}