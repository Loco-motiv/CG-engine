#pragma once

#include "Matrix.h"
#include "Object.h"
#include "Quaternion.h"
#include "SharedContext.h"
#include "Vector.h"

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

class SharedContext;

class Camera : public Object
{
public:
    Camera(GLuint l_ID,
           SharedContext* l_sharedContext,
           GLfloat l_FOV,
           GLfloat l_movementSpeed,
           GLfloat l_turnSpeed,
           sf::Vector3f l_rotation,
           sf::Vector3f l_position,
           sf::Vector3f l_scale);
    ~Camera();

    MatrixFloat GetViewMatrix();
    sf::Vector3f GetTargetPoint(GLfloat cameraDistance);
    GLfloat GetFOV() const;

    void SetFOV(GLfloat l_FOV);

    void ToggleYawGlobal();

    void HandleInput();
    void ChangeMovementSpeed(GLfloat delta);
    void Update(GLint l_elapsed);

private:
    SharedContext* m_sharedContext;

    GLfloat m_FOV;
    GLfloat m_movementSpeed;
    GLfloat m_turnSpeed;
    GLfloat m_pitchFactor = 0.1;
    GLfloat m_yawFactor   = 0.05;
    GLfloat m_isYawGlobal = true;

    GLboolean m_flagReleaseMouse = true;
    GLint m_cooldown             = 0;

    MatrixFloat m_viewMatrix;
};