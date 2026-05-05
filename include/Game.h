#pragma once

#include "Camera.h"
#include "GUI.h"
#include "InputManager.h"
#include "Matrix.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "Window.h"

#include <random>

class Game
{
public:
    Game();
    ~Game();

    void HandleInput();
    void Update();
    void Render();
    Window* GetWindow();
    void RestartClock();

private:
    SharedContext m_sharedContext;

    Window m_window;
    Graphics m_graphics;
    TextureManager m_textureManager;
    MaterialManager m_materialManager;
    MeshManager m_meshManager;
    ShaderManager m_shaderManager;
    Renderer m_renderer;
    WidgetManager m_widgetManager;
    Camera* m_camera;
    SceneManager m_sceneManager;
    InputManager m_inputManager;

    sf::Clock m_clock;
    sf::Time m_elapsed;
    int m_tickCount = 0;
    sf::Time m_elapsedFixed;

    GLboolean m_flagHideGUI = false;

    GLfloat m_cameraDistance = 10.f;

    sf::Vector3f m_targetPoint{ 0.0f, 0.0f, 0.0f };

    std::vector<std::string> m_renderMode = { "Rasterization", "PathTracing", "ReSTIR GI", "ReSTIR GI temporal", "ReSTIR GI spacial" };
    GLint m_currentRenderMode             = 2;
    std::string m_textString              = "Test";
};