#pragma once

#include "Matrix.h"
#include "Object.h"
#include "Quaternion.h"
#include "Vector.h"

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

class Camera : public Object
{
public:
    Camera();
    Camera(GLuint l_ID,
           GLfloat l_FOV,
           GLfloat l_movementSpeed,
           GLfloat l_turnSpeed,
           Transform l_transform);
    ~Camera();

    friend std::ostream& operator<<(std::ostream& os, const Camera& camera);
    friend std::istream& operator>>(std::istream& is, Camera& camera);

    MatrixFloat GetViewMatrix();
    sf::Vector3f GetTargetPoint(GLfloat cameraDistance);
    GLfloat GetFOV() const;
    GLfloat GetMovementSpeed() const { return m_movementSpeed; }
    GLfloat GetTurnSpeed() const { return m_turnSpeed; }
    GLfloat GetPitchFactor() const { return m_pitchFactor; }
    GLfloat GetYawFactor() const { return m_yawFactor; }
    GLboolean GetIsYawGlobal() const { return m_isYawGlobal; }
    sf::Vector3f GetForward() const;
    sf::Vector3f GetUp() const;
    sf::Vector3f GetRight() const;
    GLboolean HasChanges() const { return m_hasChanges; }

    void SetFOV(GLfloat l_FOV);
    void SetMovementSpeed(GLfloat l_movementSpeed) { m_movementSpeed = l_movementSpeed; }
    void SetTurnSpeed(GLfloat l_turnSpeed) { m_turnSpeed = l_turnSpeed; }
    void SetPitchFactor(GLfloat l_pitchFactor) { m_pitchFactor = l_pitchFactor; }
    void SetYawFactor(GLfloat l_yawFactor) { m_yawFactor = l_yawFactor; }
    void SetIsYawGlobal(GLboolean l_isYawGlobal) { m_isYawGlobal = l_isYawGlobal; }

    void ToggleYawGlobal();

    void HandleInput(GLint l_xDelta, GLint l_yDelta);
    void ChangeMovementSpeed(GLfloat delta);
    void Update(GLint l_elapsed);

private:
    GLfloat m_FOV;
    GLfloat m_movementSpeed;
    GLfloat m_turnSpeed;
    GLfloat m_pitchFactor   = 0.1;
    GLfloat m_yawFactor     = 0.05;
    GLboolean m_isYawGlobal = true;
    GLboolean m_hasChanges  = true;

    MatrixFloat m_viewMatrix;
};