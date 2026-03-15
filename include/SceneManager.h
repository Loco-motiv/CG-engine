#pragma once

#include "Object.h"
#include "SharedContext.h"

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>

struct ShaderContext;

class SceneManager
{
public:
    SceneManager(SharedContext* l_sharedContext);
    ~SceneManager();

    Object* MakeCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                     Material l_material);
    Object* MakeSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                       Material l_material);
    Object* MakePointLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                           LightComponent l_light);
    Object* MakeLightCube(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                          LightComponent l_light);
    Object* MakeLightSphere(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                            LightComponent l_light);
    Object* MakeDirectionalLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                                 LightComponent l_light);
    Object* MakeSpotLight(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                          LightComponent l_light);
    Object* MakeObject(sf::Vector3f l_position, sf::Vector3f l_rotation, sf::Vector3f l_scale,
                       Material l_material, MeshInfo l_mesh, LightComponent l_light);

    void HandleInput();
    void Update(GLint l_elapsed);
    void Render();

    void SetNearDistance(GLfloat l_dist) { m_nearDistance = l_dist; }
    void SetRearDistance(GLfloat l_dist) { m_rearDistance = l_dist; }
    void SetProjectionFlag(GLboolean l_flag) { m_flagProjection = l_flag; }
    void SetPickedObject(GLint l_id) { m_pickedObject = l_id; }
    void SetFlagPicked(GLboolean l_flag) { m_flagPicked = l_flag; }

    GLfloat GetNearDistance() const { return m_nearDistance; }
    GLfloat GetRearDistance() const { return m_rearDistance; }
    GLboolean IsProjectionOrthographic() const { return m_flagProjection; }

    sf::Vector2f GetPickingCords() const { return m_pickingCords; }
    GLint GetPickedObjectId() const { return m_pickedObject; }
    GLboolean IsObjectPicked() const { return m_flagPicked; }

    size_t GetObjectCount() const { return m_objects.size(); }
    const std::vector<std::unique_ptr<Object>>& GetObjects() const { return m_objects; }

    MatrixFloat GetProjectionMatrix() const { return m_projectionMatrix; }
    MatrixFloat GetViewMatrix() const { return m_viewMatrix; }

private:
    SharedContext* m_sharedContext;
    Camera m_camera;

    MatrixFloat m_projectionMatrix;
    MatrixFloat m_viewMatrix;

    GLfloat m_nearDistance = 0.1f;
    GLfloat m_rearDistance = 100.0f;

    GLboolean m_flagProjection = false;

    GLint m_objectCount     = 0;
    GLint m_spotLightCount  = 0;
    GLint m_pointLightCount = 0;

    sf::Vector2f m_pickingCords;

    GLint m_pickedObject        = 0;
    GLboolean m_flagPicked      = false;
    GLboolean m_flagPickedReady = false;
    GLint m_pickingCooldown     = 0;

    std::vector<std::unique_ptr<Object>> m_objects;

    void RenderPicking();
    void UpdateLightData(std::unique_ptr<Object>& l_object, Shader* shader, int pointLightCount, int spotLightCount);
};
