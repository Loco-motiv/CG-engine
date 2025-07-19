#include "Window.h"

Window::Window(const std::string& l_title, const sf::Vector2u& l_size)
{
    Setup(l_title, l_size);
}

Window::~Window()
{
    Destroy();
}

void Window::Setup(const std::string& l_title, const sf::Vector2u& l_size)
{
    m_windowTitle  = l_title;
    m_windowSize   = l_size;
    m_isFullscreen = false;
    m_isOver       = false;
    Create();
}

void Window::Create()
{
    sf::ContextSettings settings;
    settings.depthBits         = 32;
    settings.stencilBits       = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion      = 4; //* opengl 4.6
    settings.minorVersion      = 6; //* opengl 4.6
    settings.attributeFlags    = sf::ContextSettings::Attribute::Core;

    auto style = (m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y), m_windowTitle, style, settings);
    m_window.setVerticalSyncEnabled(true); //* frames displaying at monitor refresh rate

    m_window.setActive(true);              //* activate window for opengl rendering
}

void Window::Destroy()
{
    //* It's over ((
    m_window.close();
}

void Window::Update()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_isOver = true;
        }
        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5)
        {
            ToggleFullscreen();
        }
        else if (event.type == sf::Event::Resized)
        {
            //* adjust the viewport when the window is resized
            glViewport(0, 0, event.size.width, event.size.height); //* calculate opengl coordinates from display coordinates
            m_windowSize = m_window.getSize();                     //* update attribute for GetWindowSize
        }
    }
}

void Window::ToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    Destroy();
    Create();
}

bool Window::IsOver() { return m_isOver; }

bool Window::IsFullscreen() { return m_isFullscreen; }

sf::Vector2u Window::GetWindowSize() { return m_windowSize; }

sf::Window* Window::GetWindow() { return &m_window; }

void Window::BeginDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //* clear color buffer
}

void Window::EndDraw()
{
    glFlush(); //* force execution of GL commands
    m_window.display();
}