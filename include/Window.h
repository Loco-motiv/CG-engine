#pragma once

#include <SFML/Window.hpp>
#include <glad/glad.h>

class Window
{
public:
    Window(const std::string& l_title, const sf::Vector2u& l_size);
    ~Window();
    void BeginDraw(); //* Clear the window.
    void EndDraw();   //* Display the changes.
    void Update();
    bool IsOver();    //* It's over ((
    bool IsFullscreen();
    sf::Window* GetWindow();
    sf::Vector2u GetWindowSize();
    void ToggleFullscreen();

private:
    void Setup(const std::string& l_title, const sf::Vector2u& l_size);
    void Destroy();
    void Create();

    sf::Window m_window;

    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    GLboolean m_isOver;
    GLboolean m_isFullscreen;
};