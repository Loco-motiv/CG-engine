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

void Button::Render()
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

//* Slider

Slider::Slider(std::string l_name, GLfloat* l_currentValue, GLfloat l_minValue,
               GLfloat l_maxValue, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_minValue{ l_minValue },
      m_maxValue{ l_maxValue }, m_currentValue{ l_currentValue } {}

Slider::~Slider() {}

void Slider::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate)
{
    if (xCoordinate > m_GUI->m_leftBorder and
        xCoordinate < m_GUI->m_rightBorder and
        yCoordinate > m_topBorder - m_GUI->m_elementHeight and
        yCoordinate < m_topBorder)
    {
        *m_currentValue = ConvertNDC(xCoordinate);
    }
}
void Slider::Render()
{
    m_GUI->m_sharedContext->renderer->RenderGUI(
        (m_GUI->m_rightBorder + m_GUI->m_leftBorder) / 2.0f,
        (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
        m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap,
        m_GUI->m_elementHeight / 16.0f,
        0.3f, 0.3f, 0.3f, 1.0f);

    GLfloat currentNDC = ConvertCurrent();
    m_GUI->m_sharedContext->renderer->RenderGUI(
        currentNDC,
        (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
        m_GUI->m_elementHeight / 8.0f,
        m_GUI->m_elementHeight - m_GUI->m_elementGap * 1.8f,
        0.7f, 0.7f, 0.7f, 1.0f);

    std::string fullText  = m_name + ": " + std::to_string(*m_currentValue);
    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(
        fullText, m_GUI->m_NDCTransformerX, m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));

    m_GUI->m_sharedContext->renderer->RenderTextGUI(
        fullText,
        (m_GUI->m_leftBorder + m_GUI->m_rightBorder - textSize.x) / 2.0f,
        m_topBorder - m_GUI->m_elementGap / 2.0f,
        m_GUI->m_NDCTransformerX,
        m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY),
        0.1f, 0.1f, 0.9f, 1.0f);
}

GLfloat Slider::ConvertCurrent()
{
    return ((*m_currentValue - m_maxValue) * (m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap) /
            (m_maxValue - m_minValue)) +
           m_GUI->m_rightBorder - m_GUI->m_elementGap / 2.0f;
}

GLfloat Slider::ConvertNDC(GLfloat xCoordinate)
{
    if (xCoordinate < m_GUI->m_leftBorder + m_GUI->m_elementGap / 2.0f)
    {
        return m_minValue;
    }
    if (xCoordinate > m_GUI->m_rightBorder - m_GUI->m_elementGap / 2.0f)
    {
        return m_maxValue;
    }
    return ((m_maxValue - m_minValue) * (xCoordinate - m_GUI->m_rightBorder + m_GUI->m_elementGap / 2.0f) /
            (m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap)) +
           m_maxValue;
}

void Slider::Update(GLint l_elapsed) {}

//* HUD

HUD::HUD(std::string l_name, GLfloat* l_currentValue, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_currentValue{ l_currentValue } {}

HUD::HUD(std::string l_name, std::function<GLfloat()> l_getFunction, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_getFunction{ l_getFunction } {}

HUD::HUD(std::string l_name, std::function<std::string()> l_getStringFunction, GLfloat l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_getStringFunction{ l_getStringFunction } {}

HUD::~HUD() {}

void HUD::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) {}
void HUD::Render()
{
    if (m_name != "")
    {
        m_text = m_name + ": ";
    }
    else
    {
        m_text = "";
    }

    if (m_getStringFunction != nullptr)
    {
        m_text += m_getStringFunction();
    }
    else if (m_currentValue == nullptr)
    {
        m_text += std::to_string(m_getFunction());
    }
    else
    {
        m_text += std::to_string(*m_currentValue);
    }

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(
        m_text, m_GUI->m_NDCTransformerX, m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));

    m_GUI->m_sharedContext->renderer->RenderTextGUI(
        m_text,
        (m_GUI->m_leftBorder + m_GUI->m_elementGap / 2.0f),
        m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
        m_GUI->m_NDCTransformerX,
        m_GUI->m_NDCTransformerY,
        m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY),
        0.1f, 0.1f, 0.9f, 1.0f);
}

void HUD::Update(GLint l_elapsed) {}

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

void GUI::MakeSlider(std::string l_name, GLfloat* l_currentValue, GLfloat l_minValue, GLfloat l_maxValue)
{
    m_elements.push_back(std::make_unique<Slider>(l_name, l_currentValue, l_minValue, l_maxValue, m_topBorder, this));
    m_topBorder -= m_elementHeight;
}

void GUI::MakeHUDElement(std::string l_name, GLfloat* l_currentValue)
{
    m_elements.push_back(std::make_unique<HUD>(l_name, l_currentValue, m_topBorder, this));
    m_topBorder -= m_elementHeight;
}

void GUI::MakeHUDElement(std::string l_name, std::function<GLfloat()> l_getFunction)
{
    m_elements.push_back(std::make_unique<HUD>(l_name, l_getFunction, m_topBorder, this));
    m_topBorder -= m_elementHeight;
}

void GUI::MakeHUDElement(std::string l_name, std::function<std::string()> l_getStringFunction)
{
    m_elements.push_back(std::make_unique<HUD>(l_name, l_getStringFunction, m_topBorder, this));
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

void GUI::Update(GLint l_elapsed)
{
    for (const auto& elem : m_elements)
    {
        elem->Update(l_elapsed);
    }
}

void GUI::Render()
{
    glDisable(GL_DEPTH_TEST); //* GUI always draws on top of everything

    m_NDCTransformerX = 2.0f / m_sharedContext->window->GetWindowSize().x;
    m_NDCTransformerY = 2.0f / m_sharedContext->window->GetWindowSize().y;

    for (const auto& elem : m_elements)
    {
        elem->Render();
    }
    glEnable(GL_DEPTH_TEST);
}

sf::Vector2f GUI::ConvertScreenCoordinates(sf::Vector2i&& l_point)
{
    return sf::Vector2f(
        2.f * ((float)l_point.x / m_sharedContext->window->GetWindowSize().x) - 1.f,
        1.f - (2.f * ((float)l_point.y / m_sharedContext->window->GetWindowSize().y)));
}