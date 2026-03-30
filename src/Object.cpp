#include "Object.h"

Object::Object()
{
}

Object::Object(GLint l_ID) : m_ID{ l_ID } {}

Object::Object(GLint l_ID, Transform l_transform) : m_ID{ l_ID }, m_transform{ l_transform } { RecalculateModelMatrix(); }

Object::Object(GLint l_ID, Transform l_transform,
               std::shared_ptr<Material> l_material, std::shared_ptr<Mesh> l_mesh, LightComponent l_light)
    : m_ID{ l_ID }, m_transform{ l_transform },
      m_material{ l_material }, m_mesh{ l_mesh }, m_light{ l_light } { RecalculateModelMatrix(); }

void Object::Move(const sf::Vector3f l_transpose)
{
    m_transform.position += l_transpose;
    m_light.hasChanges    = true;
    RecalculateModelMatrix();
}

void Object::Rotate(const sf::Vector3f l_angles)
{
    Quaternion q{ l_angles };
    m_transform.rotation = q * m_transform.rotation;
    RecalculateModelMatrix();
}

void Object::Scale(const sf::Vector3f l_factors)
{
    m_transform.scale += l_factors;
    RecalculateModelMatrix();
}

std::ostream& operator<<(std::ostream& os, const Object& obj)
{
    os << obj.m_ID << " ";
    os << obj.m_transform << " ";
    std::string meshName     = obj.m_mesh ? obj.m_mesh->name : "null";
    std::string materialName = obj.m_material ? obj.m_material->name : "null";
    std::string shaderName   = obj.m_shader ? obj.m_shader->name : "null";
    os << meshName << " ";
    os << materialName << " ";
    os << shaderName << " ";
    os << obj.m_light << " ";
    return os;
}
