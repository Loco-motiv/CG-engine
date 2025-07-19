#include "GUI.h"

//* GUIElement

GUIElement::GUIElement(std::string l_name, GLdouble l_topBorder, GUI* l_GUI)
    : m_name{ l_name }, m_topBorder{ l_topBorder }, m_GUI{ l_GUI } {}

GUIElement::~GUIElement() {}

//* Button

Button::Button(std::string l_name, std::function<void()> l_callback,
               GLdouble l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_callback{ l_callback } {}

Button::~Button() {}

void Button::HandleInput(GLdouble xCoordinate, GLdouble yCoordinate)
{
    if (m_cooldown < 11) //* dirty move to prevent button flickering
    {
        if (xCoordinate > m_GUI->m_leftBorder + m_GUI->m_elementGap / 2 and
            xCoordinate < m_GUI->m_rightBorder - m_GUI->m_elementGap / 2 and
            yCoordinate > m_topBorder - m_GUI->m_elementHeight + m_GUI->m_elementGap / 2 and
            yCoordinate < m_topBorder - m_GUI->m_elementGap / 2)
        {
            m_cooldown = 270;
            m_callback();
        }
    }
}

void Button::Render()
{
    MatrixFloat transformMatrix; //! move to init
    transformMatrix.ScaleX(m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap);
    transformMatrix.ScaleY(m_GUI->m_elementHeight - m_GUI->m_elementGap);
    transformMatrix.Move((m_GUI->m_rightBorder + m_GUI->m_leftBorder) / 2.0f,
                         (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
                         0.0f); //! move to init

    m_GUI->m_sharedContext->GUIShader->SetFloatMatrix("transformMatrix",
                                                      transformMatrix.GetArray());
    if (m_cooldown > 0)
    {
        m_GUI->m_sharedContext->GUIShader->SetFloat("alpha", 0.5f);
        m_GUI->m_sharedContext->textShader->SetFloat("alpha", 0.5f);
    }

    m_GUI->m_sharedContext->graphics->DrawRectangle();

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(m_name,
                                                                                m_GUI->m_NDCTransformerX,
                                                                                m_GUI->m_NDCTransformerY,
                                                                                m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));
    m_GUI->m_sharedContext->graphics->RenderText(m_name, (m_GUI->m_leftBorder + m_GUI->m_rightBorder - textSize.x) / 2.0f,
                                                 m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
                                                 m_GUI->m_NDCTransformerX,
                                                 m_GUI->m_NDCTransformerY,
                                                 m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY), 0.1, 0.1f, 0.9f);
    if (m_cooldown > 0)
    {
        m_GUI->m_sharedContext->GUIShader->SetFloat("alpha", 1.0f);
        m_GUI->m_sharedContext->textShader->SetFloat("alpha", 1.0f);
    }
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

Slider::Slider(std::string l_name, GLdouble* l_currentValue, GLdouble l_minValue,
               GLdouble l_maxValue, GLdouble l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_minValue{ l_minValue },
      m_maxValue{ l_maxValue }, m_currentValue{ l_currentValue } {}

Slider::~Slider() {}

void Slider::HandleInput(GLdouble xCoordinate, GLdouble yCoordinate)
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
    MatrixFloat transformMatrix; //! move to init
    transformMatrix.ScaleX(m_GUI->m_rightBorder - m_GUI->m_leftBorder -
                           m_GUI->m_elementGap);
    transformMatrix.ScaleY(m_GUI->m_elementHeight / 16.0f);
    transformMatrix.Move((m_GUI->m_rightBorder + m_GUI->m_leftBorder) / 2.0f,
                         (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f,
                         0.0f); //! move to init

    m_GUI->m_sharedContext->GUIShader->SetFloatMatrix("transformMatrix",
                                                      transformMatrix.GetArray());
    m_GUI->m_sharedContext->graphics->DrawRectangle();

    MatrixFloat sliderTransformMatrix;
    sliderTransformMatrix.ScaleX(m_GUI->m_elementHeight / 8.0f);
    sliderTransformMatrix.ScaleY(m_GUI->m_elementHeight - m_GUI->m_elementGap * 1.8f);

    GLdouble currentNDC = ConvertCurrent();

    sliderTransformMatrix.Move(currentNDC, (2.0f * m_topBorder - m_GUI->m_elementHeight) / 2.0f, 0.0f);
    m_GUI->m_sharedContext->GUIShader->SetFloatMatrix("transformMatrix", sliderTransformMatrix.GetArray());
    m_GUI->m_sharedContext->graphics->DrawRectangle();

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(m_name + ": " + std::to_string(*m_currentValue),
                                                                                m_GUI->m_NDCTransformerX,
                                                                                m_GUI->m_NDCTransformerY,
                                                                                m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));
    m_GUI->m_sharedContext->graphics->RenderText(m_name + ": " + std::to_string(*m_currentValue), (m_GUI->m_leftBorder + m_GUI->m_rightBorder - textSize.x) / 2.0f,
                                                 m_topBorder - m_GUI->m_elementGap / 2.0f,
                                                 m_GUI->m_NDCTransformerX,
                                                 m_GUI->m_NDCTransformerY,
                                                 m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY), 0.1f, 0.1f, 0.9f);
}

GLdouble Slider::ConvertCurrent()
{
    return ((*m_currentValue - m_maxValue) * (m_GUI->m_rightBorder - m_GUI->m_leftBorder - m_GUI->m_elementGap) /
            (m_maxValue - m_minValue)) +
           m_GUI->m_rightBorder - m_GUI->m_elementGap / 2.0f;
}

GLdouble Slider::ConvertNDC(GLdouble xCoordinate)
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

HUD::HUD(std::string l_name, GLdouble* l_currentValue, GLdouble l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_currentValue{ l_currentValue } {}

HUD::HUD(std::string l_name, std::function<GLfloat()> l_getFunction, GLdouble l_topBorder, GUI* l_GUI)
    : GUIElement{ l_name, l_topBorder, l_GUI }, m_getFunction{ l_getFunction } {}

HUD::~HUD() {}

void HUD::HandleInput(GLdouble xCoordinate, GLdouble yCoordinate) {}

void HUD::Render()
{
    GLfloat printValue;
    if (m_currentValue == nullptr)
    {
        printValue = m_getFunction();
    }
    else
    {
        printValue = *m_currentValue;
    }

    sf::Vector2f textSize = m_GUI->m_sharedContext->graphics->GetTextDimensions(m_name + ": " + std::to_string(printValue),
                                                                                m_GUI->m_NDCTransformerX,
                                                                                m_GUI->m_NDCTransformerY,
                                                                                m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY));
    m_GUI->m_sharedContext->graphics->RenderText(m_name + ": " + std::to_string(printValue), (m_GUI->m_leftBorder + m_GUI->m_elementGap / 2.0f),
                                                 m_topBorder - (m_GUI->m_elementHeight + textSize.y) / 2.0f,
                                                 m_GUI->m_NDCTransformerX,
                                                 m_GUI->m_NDCTransformerY,
                                                 m_GUI->m_elementGap / m_GUI->m_sharedContext->graphics->GetMaxTextHeight(m_GUI->m_NDCTransformerY), 0.1, 0.1f, 0.9f);
}

void HUD::Update(GLint l_elapsed) {}

//* GUI

GUI::GUI(SharedContext* l_sharedContext, GLdouble l_leftBorder, GLdouble l_rightBorder,
         GLdouble l_elementHeight, GLdouble l_elementGap)
    : m_sharedContext{ l_sharedContext }, m_leftBorder{ l_leftBorder }, m_rightBorder{ l_rightBorder },
      m_elementHeight{ l_elementHeight }, m_elementGap{ l_elementGap } {}

GUI::~GUI()
{
    for (const auto elem : m_elements)
    {
        delete elem;
    }
}

void GUI::MakeButton(std::string l_name, std::function<void()> l_callback)
{
    Button* newButton = new Button(l_name, l_callback, m_topBorder, this);

    m_topBorder -= m_elementHeight;
    m_elements.push_back(newButton);
}

void GUI::MakeSlider(std::string l_name, GLdouble* l_currentValue, GLdouble l_minValue, GLdouble l_maxValue)
{
    Slider* newButton = new Slider(l_name, l_currentValue, l_minValue, l_maxValue, m_topBorder, this);

    m_topBorder -= m_elementHeight;
    m_elements.push_back(newButton);
}

void GUI::MakeHUDElement(std::string l_name, GLdouble* l_currentValue)
{
    HUD* newButton = new HUD(l_name, l_currentValue, m_topBorder, this);

    m_topBorder -= m_elementHeight;
    m_elements.push_back(newButton);
}

void GUI::MakeHUDElement(std::string l_name, std::function<GLfloat()> l_getFunction)
{
    HUD* newButton = new HUD(l_name, l_getFunction, m_topBorder, this);

    m_topBorder -= m_elementHeight;
    m_elements.push_back(newButton);
}

void GUI::HandleInput()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2f NDC = ConvertScreenCoordinates(sf::Mouse::getPosition(*m_sharedContext->window->GetWindow()));

        GLdouble xCoordinate = NDC.x;
        GLdouble yCoordinate = NDC.y;

        if (xCoordinate < m_leftBorder or
            xCoordinate > m_rightBorder or
            yCoordinate < m_elements.back()->m_topBorder - m_elementHeight or
            yCoordinate > m_elements.front()->m_topBorder)
        {
            return;
        }
        for (const auto elem : m_elements)
        {
            elem->HandleInput(xCoordinate, yCoordinate);
        }
    }
}

void GUI::Update(GLint l_elapsed)
{
    for (const auto elem : m_elements)
    {
        elem->Update(l_elapsed);
    }
}

void GUI::Render()
{
    m_sharedContext->GUIShader->SetFloat("alpha", 1.0f);
    m_sharedContext->textShader->SetFloat("alpha", 1.0f);
    glDisable(GL_DEPTH_TEST); //* GUI always draws on top of everything

    m_NDCTransformerX = 2.0f / m_sharedContext->window->GetWindowSize().x;
    m_NDCTransformerY = 2.0f / m_sharedContext->window->GetWindowSize().y;

    for (const auto elem : m_elements)
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