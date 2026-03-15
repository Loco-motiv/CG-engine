#pragma once

#include "Camera.h"
#include "GUI.h"
#include "Graphics.h"
#include "Window.h"

class Camera;
class GUI;
class Window;
class Shader;
class Graphics;

struct SharedContext
{
    SharedContext() : window(nullptr), graphics(nullptr), camera(nullptr), shader(nullptr), textShader(nullptr), GUIShader(nullptr) {}
    Window* window;
    Graphics* graphics;
    Camera* camera;
    GUI* GUI;
    Shader* shader;
    Shader* textShader;
    Shader* GUIShader;
    Shader* pickingShader;

    GLint m_cooldownDeadZone   = 11;
    GLint m_cooldownResetValue = 270;
};