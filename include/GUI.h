#pragma once

#include "Graphics.h"    //TODO remove
#include "Matrix.h"
#include "MeshManager.h" //TODO remove
#include "ShaderManager.h"
#include "SharedContext.h"
#include "Window.h" //TODO remove

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class GUI;
class SharedContext;

class GUIElement
{
public:
    GUIElement(std::string l_name, GLfloat l_topBorder, GUI* l_GUI);
    virtual ~GUIElement() = 0;

    virtual void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate) = 0;
    virtual void Render()                                              = 0;
    virtual void Update(GLint l_elapsed)                               = 0;

    std::string m_name;
    GLfloat m_topBorder;
    GUI* m_GUI;
};

class Button : public GUIElement
{
public:
    Button(std::string l_name, std::function<void()> l_callback, GLfloat l_topBorder, GUI* l_GUI);
    ~Button();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    GLint m_cooldown = 0;
    std::function<void()> m_callback;
};

class Slider : public GUIElement
{
public:
    Slider(std::string l_name, GLfloat* l_currentValue, GLfloat l_minValue, GLfloat l_maxValue,
           GLfloat l_topBorder, GUI* l_GUI);
    ~Slider();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    GLfloat ConvertCurrent();
    GLfloat ConvertNDC(GLfloat xCoordinate);
    GLfloat m_minValue;
    GLfloat m_maxValue;
    GLfloat* m_currentValue;
};

class HUD : public GUIElement
{
public:
    HUD(std::string l_name, GLfloat* l_currentValue, GLfloat l_topBorder, GUI* l_GUI);
    HUD(std::string l_name, std::function<GLfloat()> l_getFunction, GLfloat l_topBorder, GUI* l_GUI);
    HUD(std::string l_name, std::function<std::string()> l_getStringFunction, GLfloat l_topBorder, GUI* l_GUI);
    ~HUD();

    void HandleInput(GLfloat xCoordinate, GLfloat yCoordinate);
    void Render();
    void Update(GLint l_elapsed);

private:
    std::string m_text;
    GLfloat* m_currentValue = nullptr;
    std::function<GLfloat()> m_getFunction;
    std::function<std::string()> m_getStringFunction;
};

class GUI
{
public:
    GUI(SharedContext* l_sharedContext, GLfloat l_leftBorder, GLfloat l_rightBorder,
        GLfloat l_elementHeight, GLfloat l_elementGap);
    ~GUI();

    SharedContext* m_sharedContext;
    GLfloat m_leftBorder;
    GLfloat m_rightBorder;
    GLfloat m_elementHeight;
    GLfloat m_elementGap;

    GLfloat m_NDCTransformerX;
    GLfloat m_NDCTransformerY;

    void MakeButton(std::string l_name, std::function<void()> l_callback);
    void MakeSlider(std::string l_name, GLfloat* l_currentValue, GLfloat l_minValue, GLfloat l_maxValue);
    void MakeHUDElement(std::string l_name, GLfloat* l_currentValue);
    void MakeHUDElement(std::string l_name, std::function<GLfloat()> l_getFunction);
    void MakeHUDElement(std::string l_name, std::function<std::string()> l_getStringFunction);

    void Update(GLint l_elapsed);
    void Render();
    void HandleInput();

    sf::Vector2f ConvertScreenCoordinates(sf::Vector2i&& l_point);

    std::vector<std::unique_ptr<GUIElement>> m_elements;

private:
    GLfloat m_topBorder = 1.0f - m_elementGap / 2;
};