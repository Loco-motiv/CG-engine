#include "Window.h"

Window::Window(const std::string& l_title, const sf::Vector2u& l_size, SharedContext* l_sharedContext) : m_sharedContext(l_sharedContext)
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
    settings.majorVersion      = 4;                                                      //* opengl 4.6
    settings.minorVersion      = 6;                                                      //* opengl 4.6
    settings.attributeFlags    = sf::ContextSettings::Core | sf::ContextSettings::Debug; // TODO remove debug

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
    if (m_sharedContext->inputManager->IsWindowClosed())
    {
        m_isOver = true;
    }
    if (m_sharedContext->inputManager->IsKeyReleased(sf::Keyboard::F5))
    {
        ToggleFullscreen();
    }
    if (m_sharedContext->inputManager->IsWindowResized())
    {
        //* adjust the viewport when the window is resized
        sf::Vector2u newSize = m_sharedContext->inputManager->GetWindowSize();
        glViewport(0, 0, newSize.x, newSize.y); //* calculate opengl coordinates from display coordinates
        m_windowSize = newSize;                 //* update attribute for GetWindowSize
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
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //* clear color buffer
}

void Window::EndDraw()
{
    glFlush(); //* force execution of GL commands
    m_window.display();
}