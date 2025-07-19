#include "Game.h"

Game::Game() : m_window("CG_engine", sf::Vector2u(800, 600)), m_GUI(&m_sharedContext, 0.6f, 1.0f, 0.1f, 0.045f),
               m_camera(&m_sharedContext, 30.0f, 0.05f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f),
               m_sceneManager(&m_sharedContext)
{
    m_sharedContext.graphics   = &m_graphics;
    m_sharedContext.camera     = &m_camera;
    m_sharedContext.window     = &m_window;
    m_sharedContext.shader     = m_graphics.m_shader;
    m_sharedContext.textShader = m_graphics.m_textShader;
    m_sharedContext.GUIShader  = m_graphics.m_GUIShader;

    m_cube = m_sceneManager.MakeCube({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }, Texture::Diamond, 64, 1.0f);
    m_sceneManager.MakeCube({ 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 1.8f, 0.8f, 0.8f }, Texture::Diamond, 64, 1.0f);
    m_lightCube = m_sceneManager.MakeLightCube({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, 1.0f, 0.35f, 0.44f);
    m_sceneManager.MakeSphere({ 2.0f, 1.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.1f, 0.5f, 0.5f }, Texture::Diamond, 64, 0.8f);
    m_sceneManager.MakeSphere({ 0.0f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.5f, 0.2f }, Texture::Diamond, 64, 0.8f);
    m_sceneManager.MakeCube({ 0.0f, -2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 100.0f, 0.1f, 100.0f }, Texture::Diamond, 64, 1.0f);
    m_sceneManager.MakeDirectionalLight({ 0.0f, 0.0f, 0.0f }, { -1.57f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f }, { 0.8, 0.8f, 0.8f }, { 0.8f, 0.8f, 0.8f });
    // m_sceneManager.MakeLightSphere({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.2f, { 1.0f, 1.0f, 1.0f });

    m_GUI.MakeSlider("Turn speed", &m_turnSpeed, 0, 30);
    m_GUI.MakeButton("Projection switch", std::bind([this]()
                                                    { m_sceneManager.m_flagProjection = !m_sceneManager.m_flagProjection; }));
    // m_GUI.MakeSlider("Element gap", &(m_GUI.m_elementGap), 0, 0.11);
    // m_GUI.MakeSlider("Element height", &(m_GUI.m_elementHeight), 0.15, 0.30);
    // m_GUI.MakeSlider("Left border", &(m_GUI.m_leftBorder), -1, 0.85);
    m_GUI.MakeHUDElement("Time", std::bind([this]()
                                           { return m_elapsedFixed.asSeconds(); }));
    m_GUI.MakeHUDElement("Object count", std::bind([this]()
                                                   { return m_sceneManager.m_objectCount; }));
    m_GUI.MakeButton("Rotate switch", std::bind([this]()
                                                { m_flagRotate = !m_flagRotate; m_rotateAccumulator = 0.0f; }));
    m_GUI.MakeButton("Rotate direction switch", std::bind([this]()
                                                          { m_rotateDirection = -m_rotateDirection; }));
    m_GUI.MakeSlider("Light red value", &m_lightCube->m_diffusive.x, 0, 1);
    m_GUI.MakeSlider("Light green value", &m_lightCube->m_diffusive.y, 0, 1);
    m_GUI.MakeSlider("Light blue value", &m_lightCube->m_diffusive.z, 0, 1);
    m_GUI.MakeButton("Stop following", std::bind([this]()
                                                 { m_flagFollow = !m_flagFollow; }));
    m_GUI.MakeSlider("Radius", &m_radius, 0, 10);
    m_GUI.MakeSlider("Camera distance", &m_cameraDistance, 0, 20);

    // m_GUI.MakeSlider("Shininess", &m_cube->m_shininess, 0, 256);
    // m_GUI.MakeSlider("Near distance", &m_sceneManager.m_nearDistance, 0.1, 10);
    m_GUI.MakeButton("Spawn lightCube", std::bind([this]()
                                                  { 
            m_targetPoint = m_camera.getTargetPoint(m_cameraDistance);
            m_sceneManager.MakeLightCube(m_targetPoint, { 0.0f, 0.0f, 0.0f }, { 0.8f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }, 1.0f, 0.35f, 0.44f); }));
}

Game::~Game() {}

void Game::Update()
{
    m_window.Update();

    MatrixFloat modelMatrix;
    MatrixFloat translationMatrix;
    MatrixFloat translationMatrix2;
    MatrixFloat rotationMatrix;

    if (m_flagRotate)
    {
        if (m_flagFollow)
        {
            m_targetPoint = m_camera.getTargetPoint(m_cameraDistance);
        }

        translationMatrix.Move(m_targetPoint.x, m_targetPoint.y, m_targetPoint.z);

        translationMatrix2.Move(m_radius, m_radius, 0);
    }

    rotationMatrix.Rotate(m_rotateAccumulator, 0, 0);

    rotationMatrix = rotationMatrix * translationMatrix2;

    m_sceneManager.Update();

    MatrixFloat m_scaleMatrix;
    m_scaleMatrix.ScaleXYZ(m_cube->m_scale.x, m_cube->m_scale.y, m_cube->m_scale.z);
    m_cube->m_modelMatrix     = translationMatrix * rotationMatrix * m_scaleMatrix;
    m_cube->m_transformMatrix = m_cube->m_sceneManager->m_projectionMatrix * m_cube->m_sceneManager->m_viewMatrix * m_cube->m_modelMatrix;

    m_camera.Update(m_elapsed.asMilliseconds());
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
    m_camera.HandleInput();

    m_GUI.HandleInput();
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();

    m_elapsedFixed += m_elapsed;

    m_rotateAccumulator += m_rotateDirection * m_elapsed.asSeconds() * m_turnSpeed;
}