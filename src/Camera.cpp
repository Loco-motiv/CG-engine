#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(GLuint l_ID,
               GLfloat l_FOV,
               GLfloat l_movementSpeed,
               GLfloat l_turnSpeed,
               Transform l_transform) : Object(l_ID, l_transform),
                                        m_FOV{ l_FOV },
                                        m_movementSpeed{ l_movementSpeed },
                                        m_turnSpeed{ l_turnSpeed }
{
}

Camera::~Camera() {}

MatrixFloat Camera::GetViewMatrix()
{
    m_viewMatrix = m_transform.rotation.GetMatrix();

    MatrixFloat matrixTranslation;
    matrixTranslation.Move(-m_transform.position.x, -m_transform.position.y, -m_transform.position.z);

    m_viewMatrix = m_viewMatrix * matrixTranslation;

    return m_viewMatrix;
}

sf::Vector3f Camera::GetTargetPoint(GLfloat cameraDistance)
{
    return sf::Vector3f(-m_viewMatrix[2] * cameraDistance + m_transform.position.x,
                        -m_viewMatrix[6] * cameraDistance + m_transform.position.y,
                        -m_viewMatrix[10] * cameraDistance + m_transform.position.z);
}

GLfloat Camera::GetFOV() const
{
    return m_FOV;
}

sf::Vector3f Camera::GetForward() const
{
    return sf::Vector3f(-m_viewMatrix[2], -m_viewMatrix[6], -m_viewMatrix[10]);
}

sf::Vector3f Camera::GetUp() const
{
    return sf::Vector3f(m_viewMatrix[1], m_viewMatrix[5], m_viewMatrix[9]);
}

sf::Vector3f Camera::GetRight() const
{
    return sf::Vector3f(m_viewMatrix[0], m_viewMatrix[4], m_viewMatrix[8]);
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
    m_hasChanges        = false;
    sf::Vector3f m_move = { 0, 0, 0 };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        m_move.z += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        m_move.z -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        m_move.x -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        m_move.x += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
    {
        m_move.y += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F))
    {
        m_move.y -= 1;
    }

    sf::Vector3f m_rotationDelta{ 0.0f, 0.0f, 0.0f };

    m_rotationDelta.y += m_turnSpeed * l_xDelta * m_yawFactor;
    m_rotationDelta.x += m_turnSpeed * l_yDelta * m_pitchFactor;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) and !m_isYawGlobal)
    {
        m_rotationDelta.z -= m_turnSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) and !m_isYawGlobal)
    {
        m_rotationDelta.z += m_turnSpeed;
    }
    if (std::abs(m_rotationDelta.x) > 1e-6 || std::abs(m_rotationDelta.y) > 1e-6 || std::abs(m_rotationDelta.z) > 1e-6 || std::abs(m_move.x) > 1e-6 || std::abs(m_move.y) > 1e-6 || std::abs(m_move.z) > 1e-6)
    {
        m_hasChanges = true;
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

        m_transform.rotation = qPitchDelta * m_transform.rotation * qYawDelta;
        m_transform.rotation.Normalize();
    }
    else
    {
        //* Free Mode
        Quaternion qDelta(m_rotationDelta);
        m_transform.rotation = qDelta * m_transform.rotation;
        m_transform.rotation.Normalize();
    }

    sf::Vector3f cameraLeft(-m_viewMatrix[0], -m_viewMatrix[4], -m_viewMatrix[8]);
    sf::Vector3f cameraUp(m_viewMatrix[1], m_viewMatrix[5], m_viewMatrix[9]);
    sf::Vector3f cameraForward(-m_viewMatrix[2], -m_viewMatrix[6], -m_viewMatrix[10]);

    m_transform.position += cameraLeft * (m_movementSpeed * m_move.x);
    m_transform.position += cameraUp * (m_movementSpeed * m_move.y);
    m_transform.position += cameraForward * (m_movementSpeed * m_move.z);
}

void Camera::ChangeMovementSpeed(GLfloat delta)
{
    m_movementSpeed += delta;
}

void Camera::Update(GLint l_elapsed)
{
}

std::ostream& operator<<(std::ostream& os, const Camera& camera)

{
    os << static_cast<const Object&>(camera) << " " << camera.m_FOV << " " << camera.m_movementSpeed << " " << camera.m_turnSpeed << " " << camera.m_pitchFactor << " " << camera.m_yawFactor;
    return os;
}

std::istream& operator>>(std::istream& is, Camera& camera)
{
    GLuint ID;
    Transform transform;
    std::string meshName, materialName, shaderName;
    LightComponent light;

    is >> ID >> transform >> meshName >> materialName >> shaderName >> light;

    is >> camera.m_FOV >> camera.m_movementSpeed >> camera.m_turnSpeed >> camera.m_pitchFactor >> camera.m_yawFactor;

    camera.m_ID        = ID;
    camera.m_transform = transform;

    return is;
}