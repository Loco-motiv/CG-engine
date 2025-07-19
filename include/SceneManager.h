#pragma once

#include "SharedContext.h"

#include <vector>

enum Texture
{
    Diamond = 0
};

class SceneManager;

class Object
{
public:
    Object(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale);
    virtual ~Object() = default;

    virtual void Render() = 0;
    virtual void Update() = 0;

    SceneManager* m_sceneManager;

    sf::Vector3f m_position;
    sf::Vector3f m_rotation;
    sf::Vector3f m_scale;

    MatrixFloat m_modelMatrix;

    MatrixFloat m_transformMatrix;
};

class ObjectWithTexture : public Object
{
public:
    ObjectWithTexture(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                      Texture l_texture, GLuint l_shininess, GLfloat l_alpha);
    virtual ~ObjectWithTexture() = default;

    void Render() override;
    void Update() override;

    GLuint m_diffusiveTexture;
    GLuint m_specularTexture;

    GLuint m_shininess;

    GLfloat m_alpha;
};

class Cube : public ObjectWithTexture
{
public:
    Cube(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
         Texture l_texture, GLuint l_shininess, GLfloat l_alpha);
    ~Cube();

    void Render() override;
    void Update() override;
};

class Sphere : public ObjectWithTexture
{
public:
    Sphere(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
           Texture l_texture, GLuint l_shininess, GLfloat l_alpha);
    ~Sphere();

    void Render() override;
    void Update() override;
};

class LightSource : public Object
{
public:
    LightSource(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular);
    virtual ~LightSource() = default;

    void Update() override;
    void Render() override;

    sf::Vector3<GLdouble> m_ambient;
    sf::Vector3<GLdouble> m_diffusive;
    sf::Vector3<GLdouble> m_specular;
};

class PointLight : public LightSource
{
public:
    PointLight(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
               sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
               GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    ~PointLight();

    void Update() override;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;
};

class DirectionalLight : public LightSource
{
public:
    DirectionalLight(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                     sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular);
    ~DirectionalLight();

    void Update() override;
};

class SpotLight : public LightSource
{
public:
    SpotLight(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
              sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
              GLfloat l_cutOff, GLfloat l_outerCutOff, GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    ~SpotLight();

    void Update() override;

    GLfloat m_cutOff;
    GLfloat m_outerCutOff;

    GLfloat m_constant;
    GLfloat m_linear;
    GLfloat m_quadratic;
};

class LightCube : public PointLight
{
public:
    LightCube(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
              sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
              GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    ~LightCube();

    void Render() override;
    void Update() override;
};

class LightSphere : public PointLight
{
public:
    LightSphere(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    ~LightSphere();

    void Render() override;
    void Update() override;
};

class ObjectWithLight : public ObjectWithTexture
{
public:
    ObjectWithLight(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                    Texture l_texture, GLuint l_shininess, GLfloat l_alpha, std::vector<LightSource*> l_lights);
    ~ObjectWithLight();

    void Render() override;
    void Update() override;

    std::vector<LightSource*> m_lights;
};

class Lamp : public ObjectWithLight
{
public:
    Lamp(SceneManager* l_sceneManager, sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
         Texture l_texture, GLuint l_shininess, GLfloat l_alpha, std::vector<LightSource*> l_lights);
    ~Lamp();

    void Render() override;
    void Update() override;
};

class SceneManager
{
public:
    SceneManager(SharedContext* l_sharedContext);
    ~SceneManager();

    Cube* MakeCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                   Texture l_texture, GLuint l_shininess, GLfloat l_alpha = 1.0f);
    Sphere* MakeSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                       Texture l_texture, GLuint l_shininess, GLfloat l_alpha = 1.0f);
    PointLight* MakePointLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                               sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                               GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    LightCube* MakeLightCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                             sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                             GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    LightSphere* MakeLightSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                 sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                                 GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    DirectionalLight* MakeDirectionalLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                           sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular);
    SpotLight* MakeSpotLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                             sf::Vector3<GLdouble> l_ambient, sf::Vector3<GLdouble> l_diffusive, sf::Vector3<GLdouble> l_specular,
                             GLfloat l_cutOff, GLfloat l_outerCutOff, GLfloat l_constant, GLfloat l_linear, GLfloat l_quadratic);
    void Update();
    void Render();

    SharedContext* m_sharedContext;

    MatrixFloat m_projectionMatrix;
    MatrixFloat m_viewMatrix;

    sf::Vector3f m_cameraPosition;

    GLdouble m_nearDistance = 0.1f;

    GLboolean m_flagProjection = false;

    GLint m_spotLightCount  = 0;
    GLint m_pointLightCount = 0;

    std::vector<ObjectWithTexture*> m_objects;
    std::vector<LightSource*> m_lightSources;
};
