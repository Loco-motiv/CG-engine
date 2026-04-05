#pragma once

#include "Graphics.h" //TODO remove
#include "InputManager.h"
#include "Matrix.h"
#include "Mesh.h"
#include "MeshManager.h" //TODO remove
#include "Renderer.h"
#include "ShaderManager.h"
#include "SharedContext.h"
#include "Window.h" //TODO remove

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class WidgetBox;
class SharedContext;

class Quad
{
public:
    std::shared_ptr<Mesh> mesh;
    sf::Vector3f colour;
    GLfloat alpha;
    sf::Vector3f outlineColour;
    GLfloat outlineThickness;
    GLfloat cornerRadius;
    sf::Vector2i dimensions;

    MatrixFloat transform;
};

class WidgetText
{
public:
    std::string text;
    sf::Vector3f colour;
    GLfloat alpha;
    sf::Vector3f outlineColour;
    GLfloat outlineThickness;
    sf::Vector2i dimensions;

    sf::Vector3f position;
    sf::Vector2f scale;
};

class Widget
{
public:
    Widget(std::string l_name, GLfloat l_topBorder, WidgetBox* l_widgetBox);
    // Widget(std::string l_name);
    virtual ~Widget() = 0;

    virtual void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) = 0;
    virtual void Render() const                                        = 0;
    virtual void Update(GLint l_elapsed)                               = 0;

    std::string m_name;
    GLfloat m_topBorder;
    WidgetBox* m_widgetBox;
};

class Button : public Widget
{
public:
    Button(std::string l_name, std::function<void()> l_callback, GLfloat l_topBorder, WidgetBox* l_widgetBox);
    // Button(std::string l_name, std::function<void()> l_callback);
    ~Button();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) override;
    void Render() const override;
    void Update(GLint l_elapsed) override;

private:
    std::function<void()> m_callback;
};

template <typename T>
class Slider : public Widget
{
public:
    Slider(std::string l_name, T* l_currentValue, T l_minValue, T l_maxValue,
           GLfloat l_topBorder, WidgetBox* l_widgetBox) : Widget{ l_name, l_topBorder, l_widgetBox }, m_minValue{ l_minValue },
                                                          m_maxValue{ l_maxValue }, m_currentValue{ l_currentValue } {}
    ~Slider() {}

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) override;
    void Render() const override;
    void Update(GLint l_elapsed) override {}

private:
    GLfloat ConvertCurrent() const;
    T ConvertNDC(GLfloat xCoordinate) const;
    T m_minValue;
    T m_maxValue;
    T* m_currentValue;
};

class Label : public Widget
{
public:
    Label(std::string l_name, std::function<std::string()> l_getStringFunction, GLfloat l_topBorder, WidgetBox* l_widgetBox);
    ~Label();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) override;
    void Render() const override;
    void Update(GLint l_elapsed) override;

private:
    std::function<std::string()> m_getStringFunction;
};

class Checkbox : public Widget
{
public:
    Checkbox(std::string l_name, GLboolean* l_currentValue, GLfloat l_topBorder, WidgetBox* l_widgetBox);
    ~Checkbox();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) override;
    void Render() const override;
    void Update(GLint l_elapsed) override;

private:
    GLboolean* m_currentValue;
};

class WidgetBox
{
public:
    WidgetBox(SharedContext* l_sharedContext, GLfloat l_leftBorder, GLfloat l_rightBorder,
              GLfloat l_elementHeight, GLfloat l_elementGap);
    ~WidgetBox();

    SharedContext* m_sharedContext;
    GLfloat m_leftBorder;
    GLfloat m_rightBorder;
    GLfloat m_elementHeight;
    GLfloat m_elementGap;

    GLfloat m_NDCTransformerX;
    GLfloat m_NDCTransformerY;

    void MakeButton(std::string l_name, std::function<void()> l_callback);

    template <typename T>
    void MakeSlider(std::string l_name, T* l_currentValue, T l_minValue, T l_maxValue)
    {
        m_elements.push_back(std::make_unique<Slider<T>>(l_name, l_currentValue, l_minValue, l_maxValue, m_topBorder, this));
        m_topBorder -= m_elementHeight;
    }

    template <typename T>
    void MakeLabel(std::string l_name, T* l_currentValue)
    {
        auto getString = [name = l_name, value = l_currentValue]()
        {
            if (name == "")
            {
                return std::format("{}", *value);
            }
            return std::format("{}: {}", name, *value);
        };
        m_elements.push_back(std::make_unique<Label>(l_name, getString, m_topBorder, this));
        m_topBorder -= m_elementHeight;
    }

    template <typename Func>
    void MakeLabel(std::string l_name, Func&& l_getFunction)
    {
        auto getString = [name = l_name, l_stringFunction = std::forward<Func>(l_getFunction)]() -> std::string
        {
            auto value = l_stringFunction();

            if (name == "")
            {
                return std::format("{}", value);
            }
            return std::format("{}: {}", name, value);
        };
        m_elements.push_back(std::make_unique<Label>(l_name, getString, m_topBorder, this));
        m_topBorder -= m_elementHeight;
    }

    void MakeCheckbox(std::string l_name, GLboolean* l_currentValue);

    void Update(GLint l_elapsed);
    void Render();
    void HandleInput();

    sf::Vector2f ConvertScreenCoordinates(sf::Vector2i l_point);

    std::vector<std::unique_ptr<Widget>> m_elements;

private:
    GLfloat m_topBorder = 1.0f - m_elementGap / 2;
};

template <typename T>
void Slider<T>::HandleInput(GLfloat xCoordinate, GLfloat yCoordinate)
{
    if (xCoordinate > m_widgetBox->m_leftBorder and
        xCoordinate < m_widgetBox->m_rightBorder and
        yCoordinate > m_topBorder - m_widgetBox->m_elementHeight and
        yCoordinate < m_topBorder)
    {
        *m_currentValue = ConvertNDC(xCoordinate);
    }
}

template <typename T>
void Slider<T>::Render() const
{
    m_widgetBox->m_sharedContext->renderer->RenderGUI(
        (m_widgetBox->m_rightBorder + m_widgetBox->m_leftBorder) / 2.0f,
        (2.0f * m_topBorder - m_widgetBox->m_elementHeight) / 2.0f,
        m_widgetBox->m_rightBorder - m_widgetBox->m_leftBorder - m_widgetBox->m_elementGap,
        m_widgetBox->m_elementHeight / 16.0f,
        0.3f, 0.3f, 0.3f, 1.0f);

    GLfloat currentNDC = ConvertCurrent();
    m_widgetBox->m_sharedContext->renderer->RenderGUI(
        currentNDC,
        (2.0f * m_topBorder - m_widgetBox->m_elementHeight) / 2.0f,
        m_widgetBox->m_elementHeight / 8.0f,
        m_widgetBox->m_elementHeight - m_widgetBox->m_elementGap * 1.8f,
        0.7f, 0.7f, 0.7f, 1.0f);

    std::string fullText = std::format("{}: {}", m_name, *m_currentValue);

    sf::Vector2f textSize = m_widgetBox->m_sharedContext->graphics->GetTextDimensions(
        fullText, m_widgetBox->m_NDCTransformerX, m_widgetBox->m_NDCTransformerY,
        m_widgetBox->m_elementGap / m_widgetBox->m_sharedContext->graphics->GetMaxTextHeight(m_widgetBox->m_NDCTransformerY));

    m_widgetBox->m_sharedContext->renderer->RenderTextGUI(
        fullText,
        (m_widgetBox->m_leftBorder + m_widgetBox->m_rightBorder - textSize.x) / 2.0f,
        m_topBorder - m_widgetBox->m_elementGap / 2.0f,
        m_widgetBox->m_NDCTransformerX,
        m_widgetBox->m_NDCTransformerY,
        m_widgetBox->m_elementGap / m_widgetBox->m_sharedContext->graphics->GetMaxTextHeight(m_widgetBox->m_NDCTransformerY),
        0.1f, 0.1f, 0.9f, 1.0f);
}

template <typename T>
GLfloat Slider<T>::ConvertCurrent() const
{
    return ((static_cast<GLfloat>(*m_currentValue) - static_cast<GLfloat>(m_maxValue)) *
            (m_widgetBox->m_rightBorder - m_widgetBox->m_leftBorder - m_widgetBox->m_elementGap) /
            static_cast<GLfloat>(m_maxValue - m_minValue)) +
           m_widgetBox->m_rightBorder - m_widgetBox->m_elementGap / 2.0f;
}

template <typename T>
T Slider<T>::ConvertNDC(GLfloat xCoordinate) const
{
    if (xCoordinate < m_widgetBox->m_leftBorder + m_widgetBox->m_elementGap / 2.0f)
    {
        return m_minValue;
    }
    if (xCoordinate > m_widgetBox->m_rightBorder - m_widgetBox->m_elementGap / 2.0f)
    {
        return m_maxValue;
    }
    GLfloat result = (static_cast<GLfloat>(m_maxValue - m_minValue) * (xCoordinate - m_widgetBox->m_rightBorder + m_widgetBox->m_elementGap / 2.0f) /
                      (m_widgetBox->m_rightBorder - m_widgetBox->m_leftBorder - m_widgetBox->m_elementGap)) +
                     static_cast<GLfloat>(m_maxValue);
    return static_cast<T>(result);
}