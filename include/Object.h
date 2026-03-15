#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "Shader.h"
#include "Vector.h"

class Shader;

struct Material
{
    GLuint diffuseTexture  = 0;
    GLuint specularTexture = 0;
    GLuint shininess       = 32;
    GLfloat alpha          = 1.0f;
    sf::Vector3f colour    = sf::Vector3f(1.0f, 1.0f, 1.0f); //* defaults to white

    bool hasDiffuseTexture  = false;
    bool hasSpecularTexture = false;
};

struct MeshInfo
{
    GLuint ID     = 0;
    bool isActive = false;
};

enum LightType
{
    None = 0,
    Directional,
    Point,
    Spot
};

struct LightComponent
{
    LightType type = LightType::None;

    sf::Vector3f ambient;
    sf::Vector3f diffusive;
    sf::Vector3f specular;

    //* Point and SpotLight
    GLfloat constant  = 1.0f;
    GLfloat linear    = 0.09f;
    GLfloat quadratic = 0.032f;

    //* SpotLight
    GLfloat cutOff      = 12.5f;
    GLfloat outerCutOff = 15.5f;

    //* SpotLight and Directional
    sf::Vector3f direction;

    GLboolean hasChanges = true; //* if lightComponent is changed scene manager need to update shader
};

class Object
{
public:
    Object(GLint l_ID);
    Object(GLint l_ID, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale);
    Object(GLint l_ID, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
           Material l_material, MeshInfo l_mesh, LightComponent l_light);
    ~Object() = default;

    //* Getters
    GLint GetID() const { return m_ID; }
    sf::Vector3f GetPosition() const { return m_position; }
    sf::Vector3f GetRotation() const { return m_rotation.GetEulerAngles(); }
    sf::Vector3f GetScale() const { return m_scale; }
    MatrixFloat GetModelMatrix() const { return m_modelMatrix; }
    MatrixFloat GetTransformMatrix() const { return m_transformMatrix; }
    MeshInfo GetMesh() const { return m_mesh; }
    Material GetMaterial() const { return m_material; }
    LightComponent GetLight() const { return m_light; }

    //* Setters
    void SetPosition(const sf::Vector3f& l_position)
    {
        m_position         = l_position;
        m_light.hasChanges = true;
        RecalculateModelMatrix();
    }

    void SetRotation(const sf::Vector3f& l_rotation)
    {
        m_rotation = l_rotation;
        RecalculateModelMatrix();
    }

    void SetScale(const sf::Vector3f& l_scale)
    {
        m_scale = l_scale;
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

    void SetMesh(MeshInfo l_mesh)
    {
        m_mesh = l_mesh;
    }

    void SetMaterial(const Material& l_material)
    {
        m_material = l_material;
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

    sf::Vector3f m_position;
    Quaternion m_rotation;
    sf::Vector3f m_scale;

    Material m_material;
    MeshInfo m_mesh;
    LightComponent m_light;

    MatrixFloat m_modelMatrix;
    MatrixFloat m_transformMatrix;

    void RecalculateModelMatrix()
    {
        MatrixFloat translateMatrix;
        MatrixFloat rotateMatrix;
        MatrixFloat scaleMatrix;

        translateMatrix.Move(m_position.x, m_position.y, m_position.z);
        rotateMatrix = m_rotation.GetMatrix();
        scaleMatrix.ScaleXYZ(m_scale.x, m_scale.y, m_scale.z);

        m_modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    }
};