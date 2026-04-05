#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(800, 600)), m_graphics(),
               m_textureManager(&m_sharedContext, "resources/textures/"), m_materialManager(&m_sharedContext, "resources/materials/"),
               m_meshManager(&m_sharedContext, "resources/meshes/"), m_shaderManager(&m_sharedContext, "resources/shaderPrograms/"),
               m_renderer(&m_sharedContext), m_GUI(&m_sharedContext, 0.6f, 1.0f, 0.1f, 0.035f), m_sceneManager(&m_sharedContext)
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

    m_GUI.MakeSlider("Rotate angle", &m_rotateAngle, -1.0f, 1.0f);
    m_GUI.MakeButton("Projection switch", [this]()
                     { m_sceneManager.SetProjectionFlag(!m_sceneManager.IsProjectionOrthographic()); });
    m_GUI.MakeSlider("Element gap", &(m_GUI.m_elementGap), 0.0f, 0.11f);
    m_GUI.MakeSlider("Element height", &(m_GUI.m_elementHeight), 0.1f, 0.30f);
    m_GUI.MakeSlider("Left border", &(m_GUI.m_leftBorder), -1.0f, 0.85f);
    m_GUI.MakeLabel("Time", [this]()
                    { return m_elapsedFixed.asSeconds(); });
    m_GUI.MakeLabel("Time for tick", [this]()
                    { return m_elapsed.asSeconds(); });
    m_GUI.MakeLabel("Object count", [this]()
                    { return std::to_string(m_sceneManager.GetObjectCount()); });

    m_GUI.MakeCheckbox("Rotate", &m_flagRotate);
    m_GUI.MakeCheckbox("Follow", &m_flagFollow);
    m_GUI.MakeSlider("Radius", &m_radius, 0.0f, 10.0f);
    m_GUI.MakeSlider("Camera distance", &m_cameraDistance, 0.0f, 20.0f);
    m_GUI.MakeLabel("Picked object", [this]()
                    { return std::to_string(m_sceneManager.GetPickedObjectId()); });

    m_GUI.MakeButton("Spawn lightCube", [this]()
                     { 
            m_targetPoint = m_camera->GetTargetPoint(m_cameraDistance);
            LightComponent pointLight = LightComponent::Point(
                { 0.2f, 0.2f, 0.2f },
                { 0.8f, 0.8f, 0.8f },
                { 1.0f, 1.0f, 1.0f });
            m_sceneManager.MakeLightCube({m_targetPoint, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f }}, pointLight); });

    m_GUI.MakeButton("Toggle YawGlobal", [this]()
                     { m_camera->ToggleYawGlobal(); });
    m_GUI.MakeLabel("Camera pitch", [this]()
                    { return std::to_string(m_camera->GetRotation().x); });
    m_GUI.MakeLabel("Camera yaw", [this]()
                    { return std::to_string(m_camera->GetRotation().y); });
    m_GUI.MakeLabel("Camera roll", [this]()
                    { return std::to_string(m_camera->GetRotation().z); });
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

    m_GUI.Render();

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