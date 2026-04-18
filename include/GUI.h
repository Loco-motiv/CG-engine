#pragma once

#include "InputManager.h"
#include "Matrix.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "ShaderManager.h"
#include "SharedContext.h"
#include "Widget.h"
#include "Window.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <format>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

struct ButtonStyle : WidgetStyle
{
    void SetGlowUpColour(sf::Vector3f l_glowUpColour) { glowUpColour = l_glowUpColour; }
    void SetGlowDownColour(sf::Vector3f l_glowDownColour) { glowDownColour = l_glowDownColour; }
    sf::Vector3f glowUpColour{ 0.6f, 0.6f, 0.6f };
    sf::Vector3f glowDownColour{ 0.4f, 0.4f, 0.4f };
};

class Button : public Widget
{
public:
    Button(std::string l_name, std::function<void()> l_callback, WidgetText l_text, WidgetMesh l_mesh, Borders l_borders, WidgetHitbox l_hitbox, ButtonStyle l_style);
    ~Button();

    void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) override;
    void OnTextEntered(sf::Uint32 l_unicode) override;

    void Update() override;

private:
    ButtonStyle m_style;
    std::function<void()> m_callback;
};

struct SliderStyle : WidgetStyle
{
    SliderStyle()
    {
        meshStyle.outlineThickness = 0.0f;
    }
    void SetTrackColour(sf::Vector3f l_trackColour) { trackColour = l_trackColour; }
    void SetKnobColour(sf::Vector3f l_knobColour) { knobColour = l_knobColour; }
    void SetKnobActiveColour(sf::Vector3f l_knobActiveColour) { knobActiveColour = l_knobActiveColour; }
    sf::Vector3f trackColour{ 0.2f, 0.2f, 0.2f };
    sf::Vector3f knobColour{ 0.6f, 0.6f, 0.6f };
    sf::Vector3f knobActiveColour{ 0.7f, 0.7f, 0.5f };
};

template <typename T>
class Slider : public Widget
{
public:
    Slider(std::string l_name, T* l_valPtr, T l_minValue, T l_maxValue, WidgetText l_text,
           WidgetMesh l_trackMesh, WidgetMesh l_knobMesh, Borders l_borders,
           WidgetHitbox l_hitbox, SliderStyle l_style);

    void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override {}
    void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override {}
    void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) override {}
    void OnTextEntered(sf::Uint32 l_unicode) override {}

    void Update() override;

private:
    void UpdateValueFromMouse(float l_mouseX);
    void UpdateKnobPosition();

    T* m_valuePtr;
    T m_minValue;
    T m_maxValue;
    SliderStyle m_style;
};

template <typename T>
Slider<T>::Slider(std::string l_name, T* l_valPtr, T l_minValue, T l_maxValue, WidgetText l_text,
                  WidgetMesh l_trackMesh, WidgetMesh l_knobMesh, Borders l_borders,
                  WidgetHitbox l_hitbox, SliderStyle l_style)
    : Widget(l_name, l_borders, l_style), m_valuePtr(l_valPtr), m_minValue(l_minValue), m_maxValue(l_maxValue), m_style(l_style)
{
    m_texts.push_back(l_text);

    l_trackMesh.style        = m_style.meshStyle;
    l_trackMesh.style.colour = m_style.trackColour;
    m_meshes.push_back(l_trackMesh);

    l_knobMesh.style        = m_style.meshStyle;
    l_knobMesh.style.colour = m_style.knobColour;
    m_meshes.push_back(l_knobMesh);

    m_hitboxes.push_back(l_hitbox);
    UpdateKnobPosition();
}

template <typename T>
void Slider<T>::OnMouseDown(GLint l_index, sf::Vector2f l_mouse)
{
    m_isActive               = true;
    m_meshes[1].style.colour = m_style.knobActiveColour;
    UpdateValueFromMouse(l_mouse.x);
}

template <typename T>
void Slider<T>::OnMouseHold(GLint l_index, sf::Vector2f l_mouse)
{
    if (m_isActive)
    {
        UpdateValueFromMouse(l_mouse.x);
    }
}

template <typename T>
void Slider<T>::OnMouseUp(GLint l_index, sf::Vector2f l_mouse)
{
    m_isActive               = false;
    m_meshes[1].style.colour = m_style.knobColour;
}

template <typename T>
void Slider<T>::UpdateValueFromMouse(float l_mouseX)
{
    GLfloat left  = m_borders.left;
    GLfloat right = m_borders.right;

    GLfloat clampedValue = (l_mouseX - left) / (right - left);
    clampedValue         = std::clamp(clampedValue, 0.0f, 1.0f);

    *m_valuePtr = m_minValue + static_cast<T>(clampedValue * (m_maxValue - m_minValue));
    UpdateKnobPosition();
}

template <typename T>
void Slider<T>::UpdateKnobPosition()
{
    GLfloat clampedValue = static_cast<float>(*m_valuePtr - m_minValue) / static_cast<float>(m_maxValue - m_minValue);
    GLfloat xCoord       = m_borders.left + clampedValue * (m_borders.right - m_borders.left);

    m_meshes[1].position.x = xCoord;
}

template <typename T>
void Slider<T>::Update()
{
    m_texts[0].text = std::format("{}: {}", m_name, *m_valuePtr);
    UpdateKnobPosition();
}

class Label : public Widget
{
public:
    Label(std::string l_name, std::function<std::string()> l_getStringFunction, WidgetText l_text, Borders l_borders, WidgetStyle l_style);
    ~Label();

    void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) override;
    void OnTextEntered(sf::Uint32 l_unicode) override;

    void Update() override;

private:
    std::function<std::string()> m_getStringFunction;
};

struct DropdownStyle : WidgetStyle
{
    DropdownStyle()
    {
        meshStyle.outlineThickness = 1.0f;
    }
    void SetHeaderColour(sf::Vector3f l_colour) { headerColour = l_colour; }
    void SetOptionColour(sf::Vector3f l_colour) { optionColour = l_colour; }
    void SetHoverColour(sf::Vector3f l_colour) { hoverColour = l_colour; }
    void SetClickColour(sf::Vector3f l_colour) { clickColour = l_colour; }

    sf::Vector3f headerColour{ 0.3f, 0.3f, 0.3f };
    sf::Vector3f optionColour{ 0.2f, 0.2f, 0.2f };
    sf::Vector3f hoverColour{ 0.4f, 0.4f, 0.5f };
    sf::Vector3f clickColour{ 0.5f, 0.5f, 0.6f };
};

class Dropdown : public Widget
{
public:
    Dropdown(std::string l_name, std::vector<std::string> l_options, GLint* l_valPtr,
             std::function<void()> l_callback, WidgetText l_headerText, WidgetMesh l_headerMesh,
             Borders l_headerBorders, WidgetHitbox l_headerHitbox,
             std::vector<WidgetText> l_optTexts, std::vector<WidgetMesh> l_optMeshes,
             std::vector<WidgetHitbox> l_optHitboxes, DropdownStyle l_style);
    ~Dropdown();

    void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) override;
    void OnTextEntered(sf::Uint32 l_unicode) override;

    void Update() override;

private:
    void Toggle();
    void Open();
    void Close();

    GLint* m_valuePtr;
    std::vector<std::string> m_options;
    std::function<void()> m_callback;
    DropdownStyle m_style;
    bool m_isOpen;
};

struct TextInputStyle : WidgetStyle
{
    TextInputStyle()
    {
        meshStyle.outlineThickness = 1.0f;
    }
    void SetActiveColour(sf::Vector3f l_colour) { activeColour = l_colour; }
    void SetInactiveColour(sf::Vector3f l_colour) { inactiveColour = l_colour; }

    sf::Vector3f activeColour{ 0.3f, 0.3f, 0.4f };
    sf::Vector3f inactiveColour{ 0.2f, 0.2f, 0.2f };
};

class TextInput : public Widget
{
public:
    TextInput(std::string l_name, std::string* l_valPtr, size_t l_maxLength,
              WidgetText l_text, WidgetMesh l_mesh, Borders l_borders,
              WidgetHitbox l_hitbox, TextInputStyle l_style);
    ~TextInput();

    void OnMouseEnter(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseExit(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseDown(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseHold(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseUp(GLint l_hitboxIndex, sf::Vector2f l_NDCMouse) override;
    void OnMouseWheelScroll(GLint l_hitboxIndex, GLfloat l_delta, sf::Vector2f l_NDCMouse) override;

    void OnTextEntered(sf::Uint32 l_unicode) override; // Наш новый метод!

    void Update() override;

private:
    std::string* m_valuePtr;
    size_t m_maxLength;
    TextInputStyle m_style;
};

struct WidgetBoxStyle : WidgetStyle
{
    WidgetBoxStyle()
    {
        meshStyle.SetColour({ 0.4f, 0.4f, 0.4f });
        meshStyle.SetAlpha(0.5f);
        meshStyle.SetOutlineColour({ 0.1f, 0.1f, 0.1f });
        meshStyle.SetOutlineThickness(1);
        meshStyle.SetCornerRadius(4);
    }
};

class WidgetBox
{
public:
    WidgetBox(std::string l_name, Borders l_borders, WidgetBoxStyle l_style);
    ~WidgetBox();

    void OnWindowResize(const sf::Vector2u& l_windowSize);

    std::string m_name;
    Borders m_borders;
    WidgetBoxStyle m_style;
    WidgetMesh m_background;

    std::vector<Widget*> m_widgets;
};

struct SelectedObjectProxy
{
    Object* obj = nullptr;

    sf::Vector3f position;
    sf::Vector3f rotation;
    sf::Vector3f scale;

    std::vector<std::string> materialNames;
    std::vector<std::string> meshNames;

    GLint currentMaterialIndex = -1;
    GLint currentMeshIndex     = -1;

    void Reset()
    {
        obj = nullptr;
    }
};

class WidgetManager
{
public:
    WidgetManager(SharedContext* l_sharedContext, GLfloat l_elementHeight, GLfloat l_elementGap, WidgetBoxStyle l_style = WidgetBoxStyle());
    ~WidgetManager();

    void Update();
    void Render();
    void DispatchInput(Widget* l_widget, int l_index, const sf::Vector2f& l_mousePosition);
    void HandleInput();

    void MakeWidgetBox(std::string l_name, Borders l_borders, WidgetBoxStyle l_style = WidgetBoxStyle());
    void MakeButton(std::string l_name, std::function<void()> l_callback, std::string l_widgetBoxName, ButtonStyle l_style = ButtonStyle());
    void MakeLabel(std::string l_name, std::function<std::string()> l_getStringFunction, std::string l_widgetBoxName, WidgetStyle l_style = WidgetStyle());
    void MakeDropdown(std::string l_name, std::vector<std::string> l_options, GLint* l_valPtr, std::function<void()> l_callback, std::string l_widgetBoxName,
                      DropdownStyle l_style = DropdownStyle());
    void MakeTextInput(std::string l_name, std::string* l_valPtr, size_t l_maxLength, std::string l_widgetBoxName, TextInputStyle l_style = TextInputStyle());
    template <typename T>
    void MakeSlider(std::string l_name, T* l_valPtr, T l_minValue, T l_maxValue, std::string l_widgetBoxName, SliderStyle l_style = SliderStyle())
    {
        WidgetBox* box = m_widgetBoxes[l_widgetBoxName].get();

        GLfloat currentTop = box->m_widgets.empty() ? box->m_borders.top - m_elementGap * 0.5f : box->m_widgets.back()->GetBorders().bottom - m_elementGap;

        Borders b;
        b.top    = currentTop;
        b.bottom = b.top - m_elementHeight;
        b.left   = box->m_borders.left + m_elementGap * 0.5f;
        b.right  = box->m_borders.right - m_elementGap * 0.5f;

        WidgetMesh track;
        track.shader     = m_sharedContext->shaderManager->Get("GUI.txt");
        track.mesh       = m_sharedContext->meshManager->Get("Quad");
        track.position   = sf::Vector2f((b.left + b.right) / 2.0f, (b.top + b.bottom) / 2.0f);
        track.scale      = sf::Vector3f(b.right - b.left, (b.top - b.bottom) * 0.2f, 1.0f);
        track.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(b.right, b.bottom + (b.top - b.bottom) * 0.4f)) -
                           ConvertNDCToScreenCoordinates(sf::Vector2f(b.left, b.top - (b.top - b.bottom) * 0.4f));

        WidgetMesh knob = track;
        float knobWidth = (b.top - b.bottom) * 0.5f;
        knob.scale      = sf::Vector3f(knobWidth, b.top - b.bottom, 1.0f);
        knob.dimensions = ConvertNDCToScreenCoordinates(sf::Vector2f(b.left + knobWidth, b.bottom)) -
                          ConvertNDCToScreenCoordinates(sf::Vector2f(b.left, b.top));

        WidgetText text;
        text.shader   = m_sharedContext->shaderManager->Get("GUI.txt");
        text.position = sf::Vector2f(b.left + m_elementGap * 0.5f, b.bottom + m_elementGap * 0.5);
        text.scale    = m_elementHeight - m_elementGap;

        WidgetHitbox hitbox;
        hitbox.borders = b;

        auto slider = std::make_unique<Slider<T>>(l_name, l_valPtr, l_minValue, l_maxValue, text, track, knob, b, hitbox, l_style);
        box->m_widgets.push_back(slider.get());
        m_widgets.push_back(std::move(slider));
    }
    void ClearWidgetBox(std::string l_widgetBoxName);

private:
    sf::Vector2i ConvertNDCToScreenCoordinates(sf::Vector2f l_point);
    sf::Vector2f ConvertScreenCoordinates(sf::Vector2i l_point);

    SharedContext* m_sharedContext;

    GLfloat m_elementHeight;
    GLfloat m_elementGap;

    WidgetBoxStyle m_style;

    std::unordered_map<std::string, std::unique_ptr<WidgetBox>> m_widgetBoxes;
    std::vector<std::unique_ptr<Widget>> m_widgets;

    Widget* m_activeWidget    = nullptr;
    Widget* m_hoveredWidget   = nullptr;
    size_t m_hoveredMeshIndex = 0;

    SelectedObjectProxy currentSelection;
};