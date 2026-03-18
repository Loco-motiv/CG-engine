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
    SharedContext() {}
    Window* window;
    Graphics* graphics;
    GUI* GUI;
    Shader* shader;
    Shader* textShader;
    Shader* GUIShader;
    Shader* pickingShader;

    GLint m_cooldownDeadZone   = 11;
    GLint m_cooldownResetValue = 270;
};