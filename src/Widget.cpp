#include "Widget.h"

Widget::Widget(std::string l_name, Borders l_borders, WidgetStyle l_style) : m_name{ l_name }, m_style{ l_style }, m_borders{ l_borders } {}

Widget::~Widget() {}

void Widget::OnWindowResize(const sf::Vector2u& l_windowSize)
{
    for (auto& mesh : m_meshes)
    {
        mesh.dimensions.x = static_cast<GLint>(std::abs(mesh.scale.x) * 0.5f * l_windowSize.x);
        mesh.dimensions.y = static_cast<GLint>(std::abs(mesh.scale.y) * 0.5f * l_windowSize.y);
    }
}
