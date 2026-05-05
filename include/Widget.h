#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

struct Borders
{
    GLfloat left;
    GLfloat right;
    GLfloat top;
    GLfloat bottom;
};

struct WidgetHitbox
{
    Borders borders;
    GLboolean isIntractable = true;

    bool Contains(GLfloat xCoordinate, GLfloat yCoordinate) const
    {
        return xCoordinate > borders.left && xCoordinate < borders.right && yCoordinate > borders.bottom && yCoordinate < borders.top;
    }
};

struct WidgetMeshStyle
{
    void SetColour(sf::Vector3f l_colour) { colour = l_colour; }
    void SetAlpha(GLfloat l_alpha) { alpha = l_alpha; }
    void SetOutlineColour(sf::Vector3f l_outlineColour) { outlineColour = l_outlineColour; }
    void SetOutlineThickness(GLfloat l_outlineThickness) { outlineThickness = l_outlineThickness; }
    void SetCornerRadius(GLfloat l_cornerRadius) { cornerRadius = l_cornerRadius; }
    sf::Vector3f colour{ 0.5f, 0.5f, 0.5f };
    GLfloat alpha{ 1.0f };
    sf::Vector3f outlineColour{ 1.0f, 1.0f, 1.0f };
    GLfloat outlineThickness{ 1.0f };
    GLfloat cornerRadius{ 5.0f };
};

struct WidgetMesh
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;

    WidgetMeshStyle style;

    GLboolean isVisible = true;

    sf::Vector2i dimensions;
    sf::Vector2f position;
    sf::Vector3f scale;
};

struct WidgetTextStyle
{
    void SetColour(sf::Vector3f l_colour) { colour = l_colour; }
    void SetAlpha(GLfloat l_alpha) { alpha = l_alpha; }
    sf::Vector3f colour{ 1.0f, 1.0f, 1.0f };
    GLfloat alpha{ 1.0f };
};

struct WidgetText
{
    std::shared_ptr<Shader> shader;
    std::string text;

    WidgetTextStyle style;

    GLboolean isVisible = true;

    sf::Vector2f position;
    GLfloat scale;
};

struct WidgetStyle
{
    WidgetStyle() {}
    WidgetStyle(WidgetTextStyle l_textStyle, WidgetMeshStyle l_meshStyle) : textStyle{ l_textStyle }, meshStyle{ l_meshStyle } {}
    WidgetStyle(const WidgetStyle& l_style) : textStyle{ l_style.textStyle }, meshStyle{ l_style.meshStyle } {}
    WidgetTextStyle textStyle;
    WidgetMeshStyle meshStyle;
};

class Widget
{
public:
    Widget(std::string l_name, Borders l_borders, WidgetStyle l_style);
    virtual ~Widget() = 0;

    virtual void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)                        = 0;
    virtual void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)                         = 0;
    virtual void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)                         = 0;
    virtual void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)                         = 0;
    virtual void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse)                           = 0;
    virtual void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) = 0;
    virtual void OnTextEntered(char32_t l_unicode)                                                 = 0;
    void OnWindowResize(const sf::Vector2u& l_windowSize);
    bool IsActive() { return m_isActive; }

    const Borders& GetBorders() { return m_borders; }
    const std::vector<WidgetHitbox>& GetHitboxes() { return m_hitboxes; }
    const std::vector<WidgetMesh>& GetMeshes() { return m_meshes; }
    const std::vector<WidgetText>& GetTexts() { return m_texts; }

    virtual void Update() = 0;

protected:
    std::string m_name;
    GLboolean m_isActive = false;
    WidgetStyle m_style;
    Borders m_borders;
    std::vector<WidgetHitbox> m_hitboxes;
    std::vector<WidgetMesh> m_meshes;
    std::vector<WidgetText> m_texts;
};