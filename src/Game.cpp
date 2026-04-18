#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(800, 600), &m_sharedContext), m_graphics(),
               m_textureManager(&m_sharedContext, "resources/textures/"), m_materialManager(&m_sharedContext, "resources/materials/"),
               m_meshManager(&m_sharedContext, "resources/meshes/"), m_shaderManager(&m_sharedContext, "resources/shaderPrograms/"),
               m_renderer(&m_sharedContext), m_widgetManager(&m_sharedContext, 0.05f, 0.02), m_sceneManager(&m_sharedContext),
               m_inputManager()
{
    m_sharedContext.graphics        = &m_graphics;
    m_sharedContext.renderer        = &m_renderer;
    m_sharedContext.sceneManager    = &m_sceneManager;
    m_sharedContext.window          = &m_window;
    m_sharedContext.widgetManager   = &m_widgetManager;
    m_sharedContext.textureManager  = &m_textureManager;
    m_sharedContext.materialManager = &m_materialManager;
    m_sharedContext.meshManager     = &m_meshManager;
    m_sharedContext.shaderManager   = &m_shaderManager;
    m_sharedContext.inputManager    = &m_inputManager;

    m_sceneManager.LoadScene("scenes/Scene_30-03-2026_19-00-54.327957800.txt");
    m_camera = m_sceneManager.GetCamera();
    m_cube   = m_sceneManager.GetObject(1);

    m_widgetManager.MakeWidgetBox("Main", { -1.0f, -0.5f, 1.0f, -0.25f });
    m_widgetManager.MakeWidgetBox("InspectorBox", { 0.5f, 1.0f, 1.0f, 0.0f });
    m_widgetManager.MakeSlider("Rotate angle", &m_rotateAngle, -1.0f, 1.0f, "Main");
    m_widgetManager.MakeButton("Projection switch", [this]()
                               { m_sceneManager.SetProjectionFlag(!m_sceneManager.IsProjectionOrthographic()); }, "Main");
    m_widgetManager.MakeLabel("Time for tick", [this]()
                              { return std::string("Time: ") + std::to_string(m_elapsed.asSeconds()); }, "Main");
    m_widgetManager.MakeLabel("Object count", [this]()
                              { return std::string("Object count: ") + std::to_string(m_sceneManager.GetObjectCount()); }, "Main");
    m_widgetManager.MakeSlider("Radius", &m_radius, 0.0f, 10.0f, "Main");
    m_widgetManager.MakeButton("Spawn lightCube", [this]()
                               {
            m_targetPoint = m_camera->GetTargetPoint(m_cameraDistance);
            LightComponent pointLight = LightComponent::Point(
                { 0.2f, 0.2f, 0.2f },
                { 0.8f, 0.8f, 0.8f },
                { 1.0f, 1.0f, 1.0f });
            m_sceneManager.MakeLightCube({m_targetPoint, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f }}, pointLight); }, "Main");

    m_widgetManager.MakeDropdown(
        "Resolution",
        m_resolutions,
        &m_currentResolutionIndex,
        [this]()
        { sf::Vector2u newSize;
            std::string res = m_resolutions[m_currentResolutionIndex];
            size_t xIndex = res.find('x');
            newSize.x = std::stoi(res.substr(0, xIndex));
            newSize.y = std::stoi(res.substr(xIndex + 1));
            std::cout << newSize.x << " " << newSize.y << '\n';
            
            m_window.SetSize(newSize); },
        "Main");
    m_widgetManager.MakeButton("Toggle YawGlobal", [this]()
                               { m_camera->ToggleYawGlobal(); }, "Main");
    m_widgetManager.MakeTextInput("Player Name", &m_textString, 13, "Main");
    m_widgetManager.MakeLabel("Hero name", [this]()
                              { return m_textString; }, "Main");
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

    m_widgetManager.Update();
}

void Game::Render()
{
    m_window.BeginDraw();

    m_sceneManager.Render();

    m_widgetManager.Render();

    m_window.EndDraw();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::HandleInput()
{
    m_inputManager.HandleInput(m_window.GetWindow());

    m_widgetManager.HandleInput();

    m_sceneManager.HandleInput();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();

    m_elapsedFixed += m_elapsed;
}