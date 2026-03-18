#include "Camera.h"

Camera::Camera(GLuint l_ID,
               GLfloat l_FOV,
               GLfloat l_movementSpeed,
               GLfloat l_turnSpeed,
               sf::Vector3f l_rotation,
               sf::Vector3f l_position,
               sf::Vector3f l_scale) : Object(l_ID, l_position, l_rotation, l_scale),
                                       m_FOV{ l_FOV },
                                       m_movementSpeed{ l_movementSpeed },
                                       m_turnSpeed{ l_turnSpeed }
{
}

Camera::~Camera() {}

MatrixFloat Camera::GetViewMatrix()
{
    m_viewMatrix = m_rotation.GetMatrix();

    MatrixFloat matrixTranslation;
    matrixTranslation.Move(-m_position.x, -m_position.y, -m_position.z);

    m_viewMatrix = m_viewMatrix * matrixTranslation;

    return m_viewMatrix;
}

sf::Vector3f Camera::GetTargetPoint(GLfloat cameraDistance)
{
    return sf::Vector3f(-m_viewMatrix[2] * cameraDistance + m_position.x,
                        -m_viewMatrix[6] * cameraDistance + m_position.y,
                        -m_viewMatrix[10] * cameraDistance + m_position.z);
}

GLfloat Camera::GetFOV() const
{
    return m_FOV;
}

void Camera::SetFOV(GLfloat l_FOV)
{
    m_FOV = l_FOV;
}

void Camera::ToggleYawGlobal()
{
    m_isYawGlobal = !m_isYawGlobal;
}

void Camera::HandleInput(GLint l_xDelta, GLint l_yDelta)
{
    sf::Vector3f m_move = { 0, 0, 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        m_move.z += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        m_move.z -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        m_move.x -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        m_move.x += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
        m_move.y += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    {
        m_move.y -= 1;
    }

    sf::Vector3f m_rotationDelta{ 0.0f, 0.0f, 0.0f };

    m_rotationDelta.y += m_turnSpeed * l_xDelta * m_yawFactor;
    m_rotationDelta.x += m_turnSpeed * l_yDelta * m_pitchFactor;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) and !m_isYawGlobal)
    {
        m_rotationDelta.z -= m_turnSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) and !m_isYawGlobal)
    {
        m_rotationDelta.z += m_turnSpeed;
    }

    if (m_isYawGlobal)
    {
        //* FPS Mode
        if (m_viewMatrix[5] < 0.0f)  //* y coordinate of cameraUp vector
        {
            m_rotationDelta.y *= -1; //* camera is flipped over so its yaw is inverted
        }

        Quaternion qPitchDelta(sf::Vector3f(1, 0, 0), m_rotationDelta.x);
        Quaternion qYawDelta(sf::Vector3f(0, 1, 0), m_rotationDelta.y);

        m_rotation = qPitchDelta * m_rotation * qYawDelta;
        m_rotation.Normalize();
    }
    else
    {
        //* Free Mode
        Quaternion qDelta(m_rotationDelta);
        m_rotation = qDelta * m_rotation;
        m_rotation.Normalize();
    }

    sf::Vector3f cameraLeft(-m_viewMatrix[0], -m_viewMatrix[4], -m_viewMatrix[8]);
    sf::Vector3f cameraUp(m_viewMatrix[1], m_viewMatrix[5], m_viewMatrix[9]);
    sf::Vector3f cameraForward(-m_viewMatrix[2], -m_viewMatrix[6], -m_viewMatrix[10]);

    m_position += cameraLeft * (m_movementSpeed * m_move.x);
    m_position += cameraUp * (m_movementSpeed * m_move.y);
    m_position += cameraForward * (m_movementSpeed * m_move.z);
}

void Camera::ChangeMovementSpeed(GLfloat delta)
{
    m_movementSpeed += delta;
}

void Camera::Update(GLint l_elapsed)
{
}
