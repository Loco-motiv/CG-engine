#pragma once

#include "Matrix.h"
#include "SharedContext.h"
#include "Vector.h"

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

class SharedContext;

struct Quaternion
{
    GLfloat s; //* scalar part, s
    GLfloat x; //* vector part (x, y, z)
    GLfloat y;
    GLfloat z;

    Quaternion() : s(0), x(0), y(0), z(0) {}
    Quaternion(sf::Vector3f xyz, GLfloat l_angle);
    Quaternion(GLfloat s, GLfloat x, GLfloat y, GLfloat z);

    MatrixFloat getMatrix() const;                     //* return as 4x4 matrix

    Quaternion operator*(const Quaternion& rhs) const; //* multiplication
};

class Camera
{
public:
    Camera(SharedContext* l_sharedContext,
           GLfloat l_FOV,
           GLfloat l_movementSpeed,
           GLfloat l_turnSpeed,
           GLfloat l_yaw,
           GLfloat l_pitch,
           GLfloat l_roll,
           GLfloat l_positionX,
           GLfloat l_positionY,
           GLfloat l_positionZ);
    ~Camera();

    MatrixFloat getViewMatrix();
    sf::Vector3f getTargetPoint(GLfloat cameraDistance);
    sf::Vector3f getPosition() const;
    GLfloat getFOV() const;
    void HandleInput();
    void ChangeMovementSpeed(GLfloat delta);
    void Update(GLint l_elapsed);

private:
    MatrixFloat m_viewMatrix;

    SharedContext* m_sharedContext;

    GLfloat m_FOV;
    GLfloat m_movementSpeed;
    GLfloat m_turnSpeed;
    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_roll;
    sf::Vector3f m_position;

    GLboolean m_flagReleaseMouse = true;
    GLint m_cooldown             = 0;

    sf::Vector3f m_move;
};