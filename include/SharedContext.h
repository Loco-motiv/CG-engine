#pragma once

class Camera;
class WidgetManager;
class Window;
class Graphics;
class TextureManager;
class MaterialManager;
class MeshManager;
class ShaderManager;
class Renderer;
class InputManager;
class SceneManager;

struct SharedContext
{
    SharedContext() {}

    Window* window;
    Graphics* graphics;
    Renderer* renderer;
    SceneManager* sceneManager;
    WidgetManager* widgetManager;
    TextureManager* textureManager;
    MaterialManager* materialManager;
    MeshManager* meshManager;
    ShaderManager* shaderManager;
    InputManager* inputManager;

    int m_cooldownDeadZone   = 11;
    int m_cooldownResetValue = 270;
};