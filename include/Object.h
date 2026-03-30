#pragma once

#include "Material.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Quaternion.h"
#include "Shader.h"
#include "Vector.h"

class Shader;

struct Transform
{
    Transform() {}
    Transform(GLfloat positionX, GLfloat positionY, GLfloat positionZ) : position{ positionX, positionY, positionZ } {}
    Transform(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale)
        : position{ l_position }, rotation{ l_rotation }, scale{ l_scale } {}

    sf::Vector3f position = { 0.0f, 0.0f, 0.0f };
    Quaternion rotation   = { 1.0f, 0.0f, 0.0f, 0.0f };
    sf::Vector3f scale    = { 1.0f, 1.0f, 1.0f };
};

inline std::ostream& operator<<(std::ostream& os, const Transform& t)
{
    return os << t.position.x << " " << t.position.y << " " << t.position.z << " "
              << t.rotation.x << " " << t.rotation.y << " " << t.rotation.z << " " << t.rotation.s << " "
              << t.scale.x << " " << t.scale.y << " " << t.scale.z;
}

inline std::istream& operator>>(std::istream& is, Transform& t)
{
    return is >> t.position.x >> t.position.y >> t.position.z >> t.rotation.x >> t.rotation.y >> t.rotation.z >> t.rotation.s >> t.scale.x >> t.scale.y >> t.scale.z;
}

enum LightType
{
    None = 0,
    Directional,
    Point,
    Spot
};

struct LightComponent
{
    LightComponent() {}

    static LightComponent Directional(sf::Vector3f l_ambient, sf::Vector3f l_diffusive, sf::Vector3f l_specular,
                                      sf::Vector3f l_direction)
    {
        LightComponent directional;
        directional.type      = LightType::Directional;
        directional.ambient   = l_ambient;
        directional.diffusive = l_diffusive;
        directional.specular  = l_specular;
        directional.direction = l_direction;

        return directional;
    }

    static LightComponent Point(sf::Vector3f l_ambient, sf::Vector3f l_diffusive, sf::Vector3f l_specular,
                                GLfloat l_constant = 1.0f, GLfloat l_linear = 0.09f, GLfloat l_quadratic = 0.032f)
    {
        LightComponent point;
        point.type      = LightType::Point;
        point.ambient   = l_ambient;
        point.diffusive = l_diffusive;
        point.specular  = l_specular;

        point.constant  = l_constant;
        point.linear    = l_linear;
        point.quadratic = l_quadratic;

        return point;
    }

    static LightComponent Spotlight(sf::Vector3f l_ambient, sf::Vector3f l_diffusive, sf::Vector3f l_specular,
                                    GLfloat l_constant = 1.0f, GLfloat l_linear = 0.09f, GLfloat l_quadratic = 0.032f,
                                    GLfloat l_cutOff = 12.5f, GLfloat l_outerCutOff = 15.5f)
    {
        LightComponent spot;
        spot.type      = LightType::Spot;
        spot.ambient   = l_ambient;
        spot.diffusive = l_diffusive;
        spot.specular  = l_specular;

        spot.constant  = l_constant;
        spot.linear    = l_linear;
        spot.quadratic = l_quadratic;

        spot.cutOff      = l_cutOff;
        spot.outerCutOff = l_outerCutOff;

        return spot;
    }

    LightType type = LightType::None;

    sf::Vector3f ambient;
    sf::Vector3f diffusive;
    sf::Vector3f specular;

    //* Point and SpotLight
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;

    //* SpotLight
    GLfloat cutOff;
    GLfloat outerCutOff;

    //* SpotLight and Directional
    sf::Vector3f direction;

    GLboolean hasChanges = true; //* if lightComponent is changed scene manager need to update shader
};

inline std::ostream& operator<<(std::ostream& os, const LightComponent& l)
{
    return os << (int)l.type << " " << l.ambient.x << " " << l.ambient.y << " " << l.ambient.z << " "
              << l.diffusive.x << " " << l.diffusive.y << " " << l.diffusive.z << " "
              << l.specular.x << " " << l.specular.y << " " << l.specular.z << " "
              << l.constant << " " << l.linear << " " << l.quadratic << " "
              << l.cutOff << " " << l.outerCutOff << " "
              << l.direction.x << " " << l.direction.y << " " << l.direction.z;
}

inline std::istream& operator>>(std::istream& is, LightComponent& l)
{
    int type;
    is >> type >> l.ambient.x >> l.ambient.y >> l.ambient.z >> l.diffusive.x >> l.diffusive.y >> l.diffusive.z >> l.specular.x >> l.specular.y >> l.specular.z >> l.constant >> l.linear >> l.quadratic >> l.cutOff >> l.outerCutOff >> l.direction.x >> l.direction.y >> l.direction.z;
    l.type = static_cast<LightType>(type);
    return is;
}

class Object
{
public:
    Object();
    Object(GLint l_ID);
    Object(GLint l_ID, Transform l_transform);
    Object(GLint l_ID, Transform l_transform,
           std::shared_ptr<Material> l_material, std::shared_ptr<Mesh> l_mesh, LightComponent l_light);
    ~Object() = default;

    friend std::ostream& operator<<(std::ostream& os, const Object& obj);

    //* Getters
    GLint GetID() const { return m_ID; }
    sf::Vector3f GetPosition() const { return m_transform.position; }
    sf::Vector3f GetRotation() const { return m_transform.rotation.GetEulerAngles(); }
    sf::Vector3f GetScale() const { return m_transform.scale; }
    MatrixFloat GetModelMatrix() const { return m_modelMatrix; }
    MatrixFloat GetTransformMatrix() const { return m_transformMatrix; }
    Mesh* GetMesh() const { return m_mesh.get(); }
    Material* GetMaterial()
    {
        // if (m_material.use_count() > 1)
        // {
        //     m_material = std::make_shared<Material>(*m_material);
        // }
        return m_material.get();
    }
    // Material* GetMaterialShared() const { return m_material.get(); } //TODO realize
    Shader* GetShader() const { return m_shader.get(); }
    LightComponent GetLight() const { return m_light; }

    //* Setters
    void SetID(GLint l_ID) { m_ID = l_ID; }
    void SetPosition(const sf::Vector3f& l_position)
    {
        m_transform.position = l_position;
        m_light.hasChanges   = true;
        RecalculateModelMatrix();
    }

    void SetRotation(const sf::Vector3f& l_rotation)
    {
        m_transform.rotation = l_rotation;
        RecalculateModelMatrix();
    }

    void SetScale(const sf::Vector3f& l_scale)
    {
        m_transform.scale = l_scale;
        RecalculateModelMatrix();
    }

    void SetModelMatrix(const MatrixFloat& l_model)
    {
        m_modelMatrix = l_model;
    }

    void SetTransformMatrix(const MatrixFloat& l_transform)
    {
        m_transformMatrix = l_transform;
    }

    void UpdateTransformMatrix(const MatrixFloat& projectionViewMatrix)
    {
        m_transformMatrix = projectionViewMatrix * m_modelMatrix;
    }

    void SetMesh(std::shared_ptr<Mesh> l_mesh)
    {
        m_mesh = l_mesh;
    }

    void SetMaterial(std::shared_ptr<Material> l_material)
    {
        m_material = l_material;
    }

    void SetShader(std::shared_ptr<Shader> l_shader)
    {
        m_shader = l_shader;
    }

    void SetLight(const LightComponent& l_light)
    {
        m_light            = l_light;
        m_light.hasChanges = true;
    }

    void SetHasChangesFlag(GLboolean l_flag)
    {
        m_light.hasChanges = l_flag;
    }

    void Move(const sf::Vector3f l_transpose);
    void Rotate(const sf::Vector3f l_angles);
    void Scale(const sf::Vector3f l_factors);

protected:
    GLint m_ID;

    Transform m_transform;

    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    LightComponent m_light;

    MatrixFloat m_modelMatrix;
    MatrixFloat m_transformMatrix;

    void RecalculateModelMatrix()
    {
        MatrixFloat translateMatrix;
        MatrixFloat rotateMatrix;
        MatrixFloat scaleMatrix;

        translateMatrix.Move(m_transform.position.x, m_transform.position.y, m_transform.position.z);
        rotateMatrix = m_transform.rotation.GetMatrix();
        scaleMatrix.ScaleXYZ(m_transform.scale.x, m_transform.scale.y, m_transform.scale.z);

        m_modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    }
};