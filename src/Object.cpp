#include "Object.h"

Object::Object(GLint l_ID) : m_ID{ l_ID } {}

Object::Object(GLint l_ID, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale)
    : m_ID{ l_ID }, m_position{ l_position }, m_scale{ l_scale } { RecalculateModelMatrix(); }

Object::Object(GLint l_ID, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
               Material l_material, MeshInfo l_mesh, LightComponent l_light)
    : m_ID{ l_ID }, m_position{ l_position }, m_scale{ l_scale },
      m_material{ l_material }, m_mesh{ l_mesh }, m_light{ l_light } { RecalculateModelMatrix(); }

void Object::Move(const sf::Vector3f l_transpose)
{
    m_position         += l_transpose;
    m_light.hasChanges  = true;
    RecalculateModelMatrix();
}

void Object::Rotate(const sf::Vector3f l_angles)
{
    Quaternion q{ l_angles };
    m_rotation = q * m_rotation;
    RecalculateModelMatrix();
}

void Object::Scale(const sf::Vector3f l_factors)
{
    m_scale += l_factors;
    RecalculateModelMatrix();
}