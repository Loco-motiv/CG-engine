#pragma once

#include "Camera.h"
#include "GUI.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Object.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "SharedContext.h"
#include "Window.h"

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <vector>

enum RenderMode
{
    Rasterization = 0,
    PathTracing,
    ReSTIRGItemporal,
    ReSTIRGIspacial,
    ReSTIRGI
};

struct ShaderContext;

class SceneManager
{
public:
    SceneManager(SharedContext* l_sharedContext);
    ~SceneManager();

    void LoadScene(const fs::path& l_scenePath);
    void SaveScene(const fs::path& l_scenePath, std::string_view l_name = "Scene");

    Object* MakeCube(Transform l_transform, std::string l_materialName);
    Object* MakeSphere(Transform l_transform, std::string l_materialName);
    Object* MakePointLight(Transform l_transform, LightComponent l_light);
    Object* MakeLightCube(Transform l_transform, LightComponent l_light);
    Object* MakeLightSphere(Transform l_transform, LightComponent l_light);
    Object* MakeDirectionalLight(Transform l_transform, LightComponent l_light);
    Object* MakeSpotLight(Transform l_transform, LightComponent l_light);
    Object* MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light);
    Object* MakeObject(Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light, std::string l_shaderName);
    Object* MakeObject(GLuint l_ID, Transform l_transform, std::string l_meshName, std::string l_materialName, LightComponent l_light, std::string l_shaderName);
    void MakeObjectsFromModel(const fs::path& l_modelPath, const fs::path& l_matPath);

    void BuildBVHAndSendToGPU();

    void DeleteObject(GLint l_id);
    void ClearScene();

    void HandleInput();
    void Update(GLint l_elapsed);
    void Render();

    void SetNearDistance(GLfloat l_dist) { m_nearDistance = l_dist; }
    void SetRearDistance(GLfloat l_dist) { m_rearDistance = l_dist; }
    void SetProjectionFlag(GLboolean l_flag) { m_flagProjection = l_flag; }
    void SetPickedObject(GLint l_id) { m_pickedObject = l_id; }
    void SetFlagPicked(GLboolean l_flag) { m_flagPicked = l_flag; }
    void SetRenderMode(RenderMode l_mode) { m_renderMode = l_mode; }
    void SwitchFrameAccumulation() { m_flagFrameAccumulation = !m_flagFrameAccumulation; }

    GLfloat GetNearDistance() const { return m_nearDistance; }
    GLfloat GetRearDistance() const { return m_rearDistance; }
    GLboolean IsProjectionOrthographic() const { return m_flagProjection; }

    sf::Vector2f GetPickingCords() const { return m_pickingCords; }
    GLint GetPickedObjectId() const { return m_pickedObject; }
    GLboolean IsObjectPicked() const { return m_flagPicked; }

    bool IsBVHReady() const { return m_flagBVHReady; }

    size_t GetObjectCount() const { return m_objects.size(); }
    const std::vector<std::unique_ptr<Object>>& GetObjects() const { return m_objects; }
    Object* GetObject(GLint l_id) const
    {
        for (const auto& object : m_objects)
        {
            if (object->GetID() == l_id)
            {
                return object.get();
            }
        }
        return nullptr;
    }
    Camera* GetCamera() { return &m_camera; }

    MatrixFloat GetProjectionMatrix() const { return m_projectionMatrix; }
    MatrixFloat GetViewMatrix() const { return m_viewMatrix; }

private:
    SharedContext* m_sharedContext;
    Camera m_camera;

    MatrixFloat m_projectionMatrix;
    MatrixFloat m_viewMatrix;

    GLfloat m_nearDistance = 0.1f;
    GLfloat m_rearDistance = 10000.0f;

    GLboolean m_flagProjection   = false;
    GLboolean m_flagReleaseMouse = true;

    GLint m_lastID = 0;

    RenderMode m_renderMode = RenderMode::ReSTIRGI;

    bool m_sceneChanged = true;

    bool m_flagBVHReady = false; // TODO make empty scene rendering possible

    GLint m_stillFramesCounter   = 0;
    bool m_flagFrameAccumulation = true;

    sf::Vector2f m_pickingCords;

    GLint m_pickedObject        = 0;
    GLboolean m_flagPicked      = false;
    GLboolean m_flagPickedReady = false;

    std::vector<std::unique_ptr<Object>> m_objects;
};
