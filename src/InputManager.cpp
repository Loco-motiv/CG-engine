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
    m_enteredText     = 0;
    m_isGUITookInput  = false;

    m_mousePosition = sf::Mouse::getPosition(*l_window);

    while (const std::optional event = l_window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_windowClosed = true;
        }
        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            m_windowResized = true;
            m_windowSize    = resized->size;
        }
        else if (event->is<sf::Event::FocusGained>())
        {
            m_hasFocus = true;
        }
        else if (event->is<sf::Event::FocusLost>())
        {
            m_hasFocus = false;
        }
        else if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            m_mouseWheelDelta = scroll->delta;
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            m_keyPressed[keyPressed->code] = true;
            m_keyHeld[keyPressed->code]    = true;
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            m_keyPressed[keyReleased->code] = false;
            m_keyHeld[keyReleased->code]    = false;
        }
        else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            m_mouseButtonPressed[mousePressed->button] = true;
            m_mouseButtonHeld[mousePressed->button]    = true;
        }
        else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            m_mouseButtonPressed[mouseReleased->button] = false;
            m_mouseButtonHeld[mouseReleased->button]    = false;
        }
        else if (const auto* text = event->getIf<sf::Event::TextEntered>())
        {
            m_enteredText = text->unicode;
        }
    }
}

bool InputManager::IsKeyPressed(sf::Keyboard::Key l_key) const
{
    return m_keyPressed.contains(l_key) ? m_keyPressed.at(l_key) : false;
}

bool InputManager::IsKeyHeld(sf::Keyboard::Key l_key) const
{
    return m_keyHeld.contains(l_key) ? m_keyHeld.at(l_key) : false;
}

bool InputManager::IsKeyReleased(sf::Keyboard::Key l_key) const
{
    return m_keyPressed.contains(l_key) ? !m_keyPressed.at(l_key) : false;
}

bool InputManager::IsMouseButtonPressed(sf::Mouse::Button l_button) const
{
    return m_mouseButtonPressed.contains(l_button) ? m_mouseButtonPressed.at(l_button) : false;
}

bool InputManager::IsMouseButtonHeld(sf::Mouse::Button l_button) const
{
    return m_mouseButtonHeld.contains(l_button) ? m_mouseButtonHeld.at(l_button) : false;
}

bool InputManager::IsMouseButtonReleased(sf::Mouse::Button l_button) const
{
    return m_mouseButtonPressed.contains(l_button) ? !m_mouseButtonPressed.at(l_button) : false;
}

bool InputManager::HasFocus() const
{
    return m_hasFocus;
}

char32_t InputManager::GetEnteredText() const
{
    return m_enteredText;
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

void InputManager::GUITookInput(bool l_isGUITookInput)
{
    m_isGUITookInput = l_isGUITookInput;
}

bool InputManager::IsGUITookInput() const
{
    return m_isGUITookInput;
}
