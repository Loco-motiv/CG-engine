#pragma once

#include "Camera.h"
#include "GUI.h"
#include "InputManager.h"
#include "Matrix.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "Window.h"

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
    sf::Time m_elapsedFixed;

    GLboolean m_flagRotate = false;
    GLboolean m_flagFollow = true;

    GLfloat m_rotateAngle = 0.1f;
    GLfloat m_radius      = 1.0f;

    GLfloat m_cameraDistance = 10.f;

    sf::Vector3f m_targetPoint{ 0.0f, 0.0f, 0.0f };

    Object* m_cube;
    Object* m_lightCube;

    std::vector<std::string> m_resolutions = { "800x600", "1280x720", "1920x1080" };
    GLint m_currentResolutionIndex         = 0;
    std::string m_textString               = "Test";
};