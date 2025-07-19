#pragma once

#include "Camera.h"
#include "Graphics.h"
#include "Window.h"

class Camera;

struct SharedContext
{
    SharedContext() : window(nullptr), graphics(nullptr), camera(nullptr), shader(nullptr), textShader(nullptr), GUIShader(nullptr) {}
    Window* window;
    Graphics* graphics;
    Camera* camera;
    Shader* shader;
    Shader* textShader;
    Shader* GUIShader;
};