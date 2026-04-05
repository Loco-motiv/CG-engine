#include "GUI.h"

//* GUIElement

GUIElement::GUIElement(std::string l_name, GLfloat l_topBorder, GUI* l_GUI)
    : m_name{ l_name }, m_topBorder{ l_topBorder }, m_GUI{ l_GUI } {}

GUIElement::~GUIElement() {}

//* Button

Button::Button(std::string l_name, std::function<void()> l_callback,
               GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_callback{ l_callback } {}

Button::~Button() {}

void Button::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate)
{
    if (m_cooldown < m_GUI->m_sharedContext->m_cooldownDeadZone) //* dirty move to prevent button flickering
    {
        if (xCoordinate > m_GUI->m_leftBorder + m_GUI->m_elementGap / 2 and
            xCoordinate < m_GUI->m_rightBorder - m_GUI->m_elementGap / 2 and
            yCoordinate > m_topBorder - m_GUI->m_elementHeight + m_GUI->m_elementGap / 2 and
            yCoordinate < m_topBorder - m_GUI->m_elementGap / 2)
        {
            m_cooldown = m_GUI->m_sharedContext->m_cooldownResetValue;
            m_callback();
        }
    }
}

void Button::Render() const
{
    sf::Vector3f colour     = { 0.9f, 0.9f, 0.9f };
    sf::Vector3f textColour = { 0.1f, 0.9f, 0.1f };
    if (m_cooldown > 0)
    {
        colour     = { 0.5f, 0.5f, 0.5f };
        textColour = { 0.1f, 0.5f, 0.1f };
    }

    m_GUI->m_sharedContext->renderer->RenderGUI((m_GUI->m_leftBorder + m_GUI->m_rightBorder) / 2.0f, (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
                                                m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap, m_GUI->m_elementHeight - m_GUI->m_elementGap,
                                                colour.x, colour.y, colour.z, 1.0f); //* draw rectangle

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(m_name,
                                                                                m_GUI->m_NDCTransformerX,
                                                                                m_GUI->m_NDCTransformerY,
                                                                                m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));
    m_GUI->m_sharedContext->renderer->RenderTextGUI(m_name, (m_GUI->m_leftBorder + m_GUI->m_rightBorder - textSize.x) / 2.0f,
                                                    m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
                                                    m_GUI->m_NDCTransformerX,
                                                    m_GUI->m_NDCTransformerY,
                                                    m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY), textColour.x, textColour.y, textColour.z, 1.0f);
}

void Button::Update(GLint l_elapsed)
{
    if (m_cooldown > 0)
    {
        if (m_cooldown - l_elapsed < 0)
        {
            m_cooldown = 0;
        }
        else
        {
            m_cooldown = m_cooldown - l_elapsed;
        }
    }
}

//* Label

Label::Label(std::string l_name, std::function<std::string()> l_getStringFunction, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_getStringFunction{ l_getStringFunction } {}

Label::~Label() {}

void Label::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) {}

void Label::Render() const
{
    std::string textToRender = m_getStringFunction();

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(
        textToRender, m_GUI->m_NDCTransformerX, m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));

    m_GUI->m_sharedContext->renderer->RenderTextGUI(
        textToRender,
        (m_GUI->m_leftBorder + m_GUI->m_elementGap / 2.0f),
        m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
        m_GUI->m_NDCTransformerX,
        m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY),
        0.1f, 0.1f, 0.9f, 1.0f);
}

void Label::Update(GLint l_elapsed) {}

//* Checkbox

Checkbox::Checkbox(std::string l_name, GLboolean* l_currentValue, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement(l_name, l_topBorder, l_GUI), m_currentValue{ l_currentValue } {}

Checkbox::~Checkbox() {}

void Checkbox::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate)
{
    if (m_cooldown < m_GUI->m_sharedContext->m_cooldownDeadZone) //* dirty move to prevent button flickering
    {
        if (xCoordinate > m_GUI->m_leftBorder + m_GUI->m_elementGap / 2 and
            xCoordinate < m_GUI->m_rightBorder - m_GUI->m_elementGap / 2 and
            yCoordinate > m_topBorder - m_GUI->m_elementHeight + m_GUI->m_elementGap / 2 and
            yCoordinate < m_topBorder - m_GUI->m_elementGap / 2)
        {
            m_cooldown      = m_GUI->m_sharedContext->m_cooldownResetValue;
            *m_currentValue = !*m_currentValue;
        }
    }
}

void Checkbox::Render() const
{
    sf::Vector3f colour = *m_currentValue ? sf::Vector3f{ 0.1f, 0.9f, 0.1f } : sf::Vector3f{ 0.9f, 0.1f, 0.1f };

    m_GUI->m_sharedContext->renderer->RenderGUI((m_GUI->m_leftBorder + m_GUI->m_rightBorder) / 2.0f, (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
                                                m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap, m_GUI->m_elementHeight - m_GUI->m_elementGap,
                                                colour.x, colour.y, colour.z, 1.0f); //* draw rectangle

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(m_name,
                                                                                m_GUI->m_NDCTransformerX,
                                                                                m_GUI->m_NDCTransformerY,
                                                                                m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));
    m_GUI->m_sharedContext->renderer->RenderTextGUI(m_name, (m_GUI->m_leftBorder + m_GUI->m_rightBorder - textSize.x) / 2.0f,
                                                    m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
                                                    m_GUI->m_NDCTransformerX,
                                                    m_GUI->m_NDCTransformerY,
                                                    m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY), 0.1f, 0.1f, 0.9f, 1.0f);
}

void Checkbox::Update(GLint l_elapsed)
{
    if (m_cooldown > 0)
    {
        if (m_cooldown - l_elapsed < 0)
        {
            m_cooldown = 0;
        }
        else
        {
            m_cooldown = m_cooldown - l_elapsed;
        }
    }
}

//* GUI

GUI::GUI(SharedContext* l_sharedContext, GLfloat l_leftBorder, GLfloat l_rightBorder,
         GLfloat l_elementHeight, GLfloat l_elementGap)
    : m_sharedContext{ l_sharedContext }, m_leftBorder{ l_leftBorder }, m_rightBorder{ l_rightBorder },
      m_elementHeight{ l_elementHeight }, m_elementGap{ l_elementGap } {}

GUI::~GUI()
{
}

void GUI::MakeButton(std::string l_name, std::function<void()> l_callback)
{
    m_elements.push_back(std::make_unique<Button>(l_name, l_callback, m_topBorder, this));
    m_topBorder -= m_elementHeight;
}

void GUI::HandleInput()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2f NDC = ConvertScreenCoordinates(sf::Mouse::getPosition(*m_sharedContext->window->GetWindow()));

        GLfloat xCoordinate = NDC.x;
        GLfloat yCoordinate = NDC.y;

        if (xCoordinate < m_leftBorder or
            xCoordinate > m_rightBorder or
            yCoordinate < m_elements.back()->m_topBorder - m_elementHeight or
            yCoordinate > m_elements.front()->m_topBorder)
        {
            return;
        }
        for (const auto& elem : m_elements)
        {
            elem->HandleInput(xCoordinate, yCoordinate);
        }
    }
}

void GUI::MakeCheckbox(std::string l_name, GLboolean* l_currentValue)
{
    m_elements.push_back(std::make_unique<Checkbox>(l_name, l_currentValue, m_topBorder, this));
    m_topBorder -= m_elementHeight;
}

void GUI::Update(GLint l_elapsed)
{
    for (const auto& elem : m_elements)
    {
        elem->Update(l_elapsed);
    }
}

void GUI::Render()
{
    m_sharedContext->renderer->BeginGUIRender();

    m_NDCTransformerX = 2.0f / m_sharedContext->window->GetWindowSize().x;
    m_NDCTransformerY = 2.0f / m_sharedContext->window->GetWindowSize().y;

    for (const auto& elem : m_elements)
    {
        elem->Render();
    }
    m_sharedContext->renderer->EndGUIRender();
}

sf::Vector2f GUI::ConvertScreenCoordinates(sf::Vector2i&& l_point)
{
    return sf::Vector2f(
        2.f * ((float)l_point.x / m_sharedContext->window->GetWindowSize().x) - 1.f,
        1.f - (2.f * ((float)l_point.y / m_sharedContext->window->GetWindowSize().y)));
}
