#pragma once

#include <SFML/Window.hpp>
#include <unordered_map>

class InputManager
{
public:
    InputManager();
    ~InputManager();

    void HandleInput(sf::Window* l_window);

    bool IsKeyPressed(sf::Keyboard::Key l_key) const;
    bool IsKeyReleased(sf::Keyboard::Key l_key) const;
    bool IsMouseButtonPressed(sf::Mouse::Button l_button) const;
    bool IsMouseButtonReleased(sf::Mouse::Button l_button) const;

    bool HasFocus() const;

    float GetMouseWheelDelta() const;

    sf::Vector2i GetMousePosition() const;

    bool IsWindowClosed() const;
    bool IsWindowResized() const;
    sf::Vector2u GetWindowSize() const;

private:
    std::unordered_map<sf::Keyboard::Key, bool> m_keyPressed;
    std::unordered_map<sf::Mouse::Button, bool> m_mouseButtonPressed;

    bool m_windowClosed;
    bool m_windowResized;
    sf::Vector2u m_windowSize;

    bool m_hasFocus;

    sf::Vector2i m_mousePosition;
    float m_mouseWheelDelta;
};