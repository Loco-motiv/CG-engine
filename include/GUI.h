#pragma once

#include "Matrix.h"
#include "SharedContext.h"

#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <string>
#include <vector>

class GUI;

class GUIElement
{
public:
    GUIElement(std::string l_name, GLdouble l_topBorder, GUI* l_GUI);
    virtual ~GUIElement() = 0;

    virtual void HandleInput(GLdouble xCoordinate, GLdouble yCoordinate) = 0;
    virtual void Render()                                                = 0;
    virtual void Update(GLint l_elapsed)                                 = 0;

    std::string m_name;
    GLdouble m_topBorder;
    GUI* m_GUI;
};

class Button : public GUIElement
{
public:
    Button(std::string l_name, std::function<void()> l_callback, GLdouble l_topBorder, GUI* l_GUI);
    ~Button();

    void HandleInput(GLdouble xCoordinate, GLdouble yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    GLint m_cooldown = 0;
    std::function<void()> m_callback;
};

class Slider : public GUIElement
{
public:
    Slider(std::string l_name, GLdouble* l_currentValue, GLdouble l_minValue, GLdouble l_maxValue,
           GLdouble l_topBorder, GUI* l_GUI);
    ~Slider();

    void HandleInput(GLdouble xCoordinate, GLdouble yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    GLdouble ConvertCurrent();
    GLdouble ConvertNDC(GLdouble xCoordinate);
    GLdouble m_minValue;
    GLdouble m_maxValue;
    GLdouble* m_currentValue;
};

class HUD : public GUIElement
{
public:
    HUD(std::string l_name, GLdouble* l_currentValue, GLdouble l_topBorder, GUI* l_GUI);
    HUD(std::string l_name, std::function<GLfloat()> l_getFunction, GLdouble l_topBorder, GUI* l_GUI);
    ~HUD();

    void HandleInput(GLdouble xCoordinate, GLdouble yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    GLdouble* m_currentValue = nullptr;
    std::function<GLfloat()> m_getFunction;
};

class GUI
{
public:
    GUI(SharedContext* l_sharedContext, GLdouble l_leftBorder, GLdouble l_rightBorder,
        GLdouble l_elementHeight, GLdouble l_elementGap);
    ~GUI();

    SharedContext* m_sharedContext;
    GLdouble m_leftBorder;
    GLdouble m_rightBorder;
    GLdouble m_elementHeight;
    GLdouble m_elementGap;

    GLfloat m_NDCTransformerX;
    GLfloat m_NDCTransformerY;

    void MakeButton(std::string l_name, std::function<void()> l_callback);
    void MakeSlider(std::string l_name, GLdouble* l_currentValue, GLdouble l_minValue, GLdouble l_maxValue);
    void MakeHUDElement(std::string l_name, GLdouble* l_currentValue);
    void MakeHUDElement(std::string l_name, std::function<GLfloat()> l_getFunction);

    void Update(GLint l_elapsed);
    void Render();
    void HandleInput();

    sf::Vector2f ConvertScreenCoordinates(sf::Vector2i&& l_point);

private:
    GLdouble m_topBorder = 1.0f - m_elementGap / 2;
    std::vector<GUIElement*> m_elements;
};