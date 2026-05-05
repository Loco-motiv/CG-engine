#pragma once

#include "InputManager.h"
#include "SharedContext.h"

#include <SFML/Window.hpp>
#include <glad/gl.h>
#include <iostream>

class Window
{
public:
    Window(const std::string& l_title, const sf::Vector2u& l_size, const GLboolean l_isFullscreen, SharedContext* l_sharedContext);
    ~Window();
    void BeginDraw(); //* Clear the window.
    void EndDraw();   //* Display the changes.
    void Update();
    bool IsOver();    //* It's over ((
    bool IsFullscreen();
    sf::Window* GetWindow();
    sf::Vector2u GetWindowSize();
    void ToggleFullscreen();
    void SetSize(const sf::Vector2u& l_size);

private:
    void Setup(const std::string& l_title, const sf::Vector2u& l_size, const GLboolean l_isFullscreen);
    void Destroy();
    void Create();

    sf::Window m_window;

    SharedContext* m_sharedContext;

    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    GLboolean m_isOver;
    GLboolean m_isFullscreen;
};