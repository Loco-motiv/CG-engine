#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(1920, 1080), true, &m_sharedContext), m_graphics(),
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

    m_meshManager.InitPreload();
    m_materialManager.InitPreload();

    m_sceneManager.LoadScene("scenes/Scene_02-05-2026_19-23-29.968695600.txt");
    m_camera = m_sceneManager.GetCamera();

    m_widgetManager.MakeWidgetBox("Main", { -1.0f, -0.5f, 1.0f, -0.25f });
    m_widgetManager.MakeWidgetBox("InspectorBox", { 0.5f, 1.0f, 1.0f, 0.0f });
    m_widgetManager.MakeButton("Projection switch", [this]()
                               { m_sceneManager.SetProjectionFlag(!m_sceneManager.IsProjectionOrthographic()); }, "Main");
    m_widgetManager.MakeLabel("Time for tick", [this]()
                              { return std::string("Time: ") + std::to_string(m_elapsed.asSeconds()); }, "Main");
    m_widgetManager.MakeLabel("Time for tick(avg)", [this]()
                              { return std::string("Time(avg): ") + std::to_string(m_elapsedFixed.asSeconds() / (float)m_tickCount); }, "Main");
    m_widgetManager.MakeButton("Time reset", [this]()
                               { m_elapsedFixed = sf::Time::Zero; m_tickCount = 0; }, "Main");
    m_widgetManager.MakeLabel("Object count", [this]()
                              { return std::string("Object count: ") + std::to_string(m_sceneManager.GetObjectCount()); }, "Main");

    m_widgetManager.MakeDropdown(
        "Render mode",
        m_renderMode,
        &m_currentRenderMode,
        [this]()
        {
            if (m_currentRenderMode == 0)
            {
                m_sceneManager.SetRenderMode(RenderMode::Rasterization);
            }
            else if (m_currentRenderMode == 1)
            {
                m_sceneManager.SetRenderMode(RenderMode::PathTracing);
            }
            else if (m_currentRenderMode == 2)
            {
                m_sceneManager.SetRenderMode(RenderMode::ReSTIRGI);
            }
            else if (m_currentRenderMode == 3)
            {
                m_sceneManager.SetRenderMode(RenderMode::ReSTIRGItemporal);
            }
            else if (m_currentRenderMode == 4)
            {
                m_sceneManager.SetRenderMode(RenderMode::ReSTIRGIspacial);
            }
        },
        "Main");
    m_widgetManager.MakeButton("Switch frame accumulation", [this]()
                               { m_sceneManager.SwitchFrameAccumulation(); }, "Main");
    m_widgetManager.MakeButton("Toggle YawGlobal", [this]()
                               { m_camera->ToggleYawGlobal(); }, "Main");
    m_widgetManager.MakeTextInput("Player Name", &m_textString, 13, "Main");
    m_widgetManager.MakeLabel("Hero name", [this]()
                              { return m_textString; }, "Main");
    m_widgetManager.MakeButton("Spawn model", [this]()
                               { m_sceneManager.MakeObjectsFromModel("resources/meshes/sponza.obj", "resources/materials/sponza.mtl"); }, "Main");
    m_widgetManager.MakeButton("Make directional light", [this]()
                               {LightComponent directionalLight = LightComponent::Directional(
            { 0.8f, 0.8f, 0.8f },
            { 0.8f, 0.8f, 0.8f },
            { 1.0f, 1.0f, 1.0f },
            { -0.2f, -1.0f, -0.3f });
        m_sceneManager.MakeDirectionalLight({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }}, directionalLight); }, "Main");
    m_widgetManager.MakeButton("Rebuild BVH", [this]()
                               { m_sceneManager.BuildBVHAndSendToGPU(); }, "Main");
    m_widgetManager.MakeButton("Clear scene", [this]()
                               { m_sceneManager.ClearScene(); }, "Main");
}

Game::~Game() {}

void Game::Update()
{
    m_window.Update();

    m_sceneManager.Update(m_elapsed.asMilliseconds());

    if (!m_flagHideGUI)
    {
        m_widgetManager.Update();
    }
}

void Game::Render()
{
    m_window.BeginDraw();

    m_sceneManager.Render();

    if (!m_flagHideGUI)
    {
        m_widgetManager.Render();
    }

    m_window.EndDraw();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::HandleInput()
{
    m_inputManager.HandleInput(m_window.GetWindow());

    if (m_inputManager.IsKeyReleased(sf::Keyboard::Key::G))
    {
        m_flagHideGUI = !m_flagHideGUI;
    }

    if (!m_flagHideGUI)
    {
        m_widgetManager.HandleInput();
    }
    m_sceneManager.HandleInput();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();
    m_tickCount++;

    m_elapsedFixed += m_elapsed;
}