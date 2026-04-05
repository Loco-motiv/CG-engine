#include "InputManager.h"

InputManager::InputManager()
{
    m_windowClosed    = false;
    m_windowResized   = false;
    m_mouseWheelDelta = 0.0f;
}

InputManager::~InputManager() {}

void InputManager::HandleInput(sf::Window* l_window)
{
    m_keyPressed.clear();
    m_mouseButtonPressed.clear();
    m_windowClosed    = false;
    m_windowResized   = false;
    m_mouseWheelDelta = 0.0f;

    m_mousePosition = sf::Mouse::getPosition(*l_window);

    sf::Event event;
    while (l_window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_windowClosed = true;
        }
        else if (event.type == sf::Event::Resized)
        {
            m_windowResized = true;
            m_windowSize    = sf::Vector2u(event.size.width, event.size.height);
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            m_hasFocus = true;
        }
        else if (event.type == sf::Event::LostFocus)
        {
            m_hasFocus = false;
        }
        else if (event.type == sf::Event::MouseWheelScrolled)
        {
            m_mouseWheelDelta = event.mouseWheelScroll.delta;
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            m_keyPressed[event.key.code] = true;
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            m_keyPressed[event.key.code] = false;
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            m_mouseButtonPressed[event.mouseButton.button] = true;
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            m_mouseButtonPressed[event.mouseButton.button] = false;
        }
    }
}

bool InputManager::IsKeyPressed(sf::Keyboard::Key l_key) const
{
    return m_keyPressed.contains(l_key) ? m_keyPressed.at(l_key) : false;
}

bool InputManager::IsKeyReleased(sf::Keyboard::Key l_key) const
{
    return m_keyPressed.contains(l_key) ? !m_keyPressed.at(l_key) : false;
}

bool InputManager::IsMouseButtonPressed(sf::Mouse::Button l_button) const
{
    return m_mouseButtonPressed.contains(l_button) ? m_mouseButtonPressed.at(l_button) : false;
}

bool InputManager::IsMouseButtonReleased(sf::Mouse::Button l_button) const
{
    return m_mouseButtonPressed.contains(l_button) ? !m_mouseButtonPressed.at(l_button) : false;
}

bool InputManager::HasFocus() const
{
    return m_hasFocus;
}

float InputManager::GetMouseWheelDelta() const
{
    return m_mouseWheelDelta;
}

sf::Vector2i InputManager::GetMousePosition() const
{
    return m_mousePosition;
}

bool InputManager::IsWindowClosed() const
{
    return m_windowClosed;
}

bool InputManager::IsWindowResized() const
{
    return m_windowResized;
}

sf::Vector2u InputManager::GetWindowSize() const
{
    return m_windowSize;
}
