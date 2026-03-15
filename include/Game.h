#pragma once

#include "Camera.h"
#include "GUI.h"
#include "Matrix.h"
#include "SceneManager.h"

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
    GUI m_GUI;
    Camera* m_camera;
    SceneManager m_sceneManager;

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
};