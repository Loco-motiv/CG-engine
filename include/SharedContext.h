#pragma once

class Camera;
class GUI;
class Window;
class Graphics;
class TextureManager;
class MaterialManager;
class MeshManager;
class ShaderManager;
class Renderer;

struct SharedContext
{
    SharedContext() {}

    Window* window;
    Graphics* graphics;
    Renderer* renderer;
    GUI* GUI;
    TextureManager* textureManager;
    MaterialManager* materialManager;
    MeshManager* meshManager;
    ShaderManager* shaderManager;

    int m_cooldownDeadZone   = 11;
    int m_cooldownResetValue = 270;
};