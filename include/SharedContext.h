#pragma once

#include "Camera.h"
#include "GUI.h"
#include "Graphics.h"
#include "Window.h"

class Camera;
class GUI;

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
};