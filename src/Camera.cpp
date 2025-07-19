#include "Camera.h"

Quaternion::Quaternion(sf::Vector3f xyz, GLfloat l_angle)
{
    float sine = sinf(l_angle); //* angle is radian
    s          = cosf(l_angle);
    x          = xyz.x * sine;
    y          = xyz.y * sine;
    z          = xyz.z * sine;
}

Quaternion::Quaternion(GLfloat l_s, GLfloat l_x, GLfloat l_y, GLfloat l_z)
{
    s = l_s;
    x = l_x;
    y = l_y;
    z = l_z;
}

MatrixFloat Quaternion::getMatrix() const
{
    float x2  = x + x;
    float y2  = y + y;
    float z2  = z + z;
    float xx2 = x * x2;
    float xy2 = x * y2;
    float xz2 = x * z2;
    float yy2 = y * y2;
    float yz2 = y * z2;
    float zz2 = z * z2;
    float sx2 = s * x2;
    float sy2 = s * y2;
    float sz2 = s * z2;

    MatrixFloat result;

    result.Set(1 - (yy2 + zz2), xy2 + sz2, xz2 - sy2, 0,
               xy2 - sz2, 1 - (xx2 + zz2), yz2 + sx2, 0,
               xz2 + sy2, yz2 - sx2, 1 - (xx2 + yy2), 0,
               0, 0, 0, 1);

    return result;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    sf::Vector3f v1(x, y, z);                             //* vector part of q
    sf::Vector3f v2(rhs.x, rhs.y, rhs.z);                 //* vector part of q'

    sf::Vector3f cross = Cross(v1, v2);                   //* v x v' (cross product)
    GLfloat dot        = Dot(v1, v2);                     //* v . v' (inner product)
    sf::Vector3f v3    = cross + (s * v2) + (rhs.s * v1); //* v x v' + sv' + s'v

    return Quaternion(s * rhs.s - dot, v3.x, v3.y, v3.z);
}

Camera::Camera(SharedContext* l_sharedContext,
               GLfloat l_FOV,
               GLfloat l_movementSpeed,
               GLfloat l_turnSpeed,
               GLfloat l_yaw,
               GLfloat l_pitch,
               GLfloat l_roll,
               GLfloat l_positionX,
               GLfloat l_positionY,
               GLfloat l_positionZ) : m_sharedContext{ l_sharedContext },
                                      m_FOV{ l_FOV },
                                      m_movementSpeed{ l_movementSpeed },
                                      m_turnSpeed{ l_turnSpeed },
                                      m_yaw{ l_yaw },
                                      m_pitch{ l_pitch },
                                      m_roll{ l_roll },
                                      m_position{ l_positionX, l_positionY, l_positionZ } {}

Camera::~Camera() {}

MatrixFloat Camera::getViewMatrix()
{
    MatrixFloat matrixRotation;
    MatrixFloat matrixTranslation;

    Quaternion qx = Quaternion(sf::Vector3f(1, 0, 0), m_pitch * 0.5f * acos(-1) / 180.0f);
    Quaternion qy = Quaternion(sf::Vector3f(0, 1, 0), -m_yaw * 0.5f * acos(-1) / 180.0f);
    Quaternion qz = Quaternion(sf::Vector3f(0, 0, 1), m_roll * 0.5f * acos(-1) / 180.0f);
    Quaternion q  = qx * qy * qz; //* rotation order: z -> y -> x

    //* convert quaternion to rotation matrix
    matrixRotation = q.getMatrix();

    sf::Vector3f cameraLeft(-m_viewMatrix[0], -m_viewMatrix[4], -m_viewMatrix[8]);
    sf::Vector3f cameraUp(m_viewMatrix[1], m_viewMatrix[5], m_viewMatrix[9]);
    sf::Vector3f cameraForward(-m_viewMatrix[2], -m_viewMatrix[6], -m_viewMatrix[10]);

    sf::Vector3f deltaMovement  = m_movementSpeed * cameraLeft * m_move.x;
    deltaMovement              += m_movementSpeed * cameraUp * m_move.y;
    deltaMovement              += m_movementSpeed * cameraForward * m_move.z;

    m_position += deltaMovement;

    matrixTranslation.Move(-m_position.x, -m_position.y, -m_position.z);

    m_viewMatrix = matrixRotation * matrixTranslation;

    return m_viewMatrix;
}

sf::Vector3f Camera::getTargetPoint(GLfloat cameraDistance)
{
    return sf::Vector3f(-m_viewMatrix[2] * cameraDistance + m_position.x,
                        -m_viewMatrix[6] * cameraDistance + m_position.y,
                        -m_viewMatrix[10] * cameraDistance + m_position.z);
}

sf::Vector3f Camera::getPosition() const
{
    return m_position;
}

GLfloat Camera::getFOV() const
{
    return m_FOV;
}

void Camera::HandleInput()
{
    m_move = { 0, 0, 0 };
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

    if (!m_flagReleaseMouse)
    {
        sf::Vector2i mousePosition  = sf::Mouse::getPosition(*m_sharedContext->window->GetWindow());
        sf::Vector2i windowCenter   = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2, m_sharedContext->window->GetWindowSize().y / 2);
        m_yaw                      -= m_turnSpeed * (mousePosition.x - windowCenter.x) * 0.05f;
        m_pitch                    += m_turnSpeed * (mousePosition.y - windowCenter.y) * 0.1f;
        sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) and m_cooldown < 11)
    {
        m_cooldown         = 270;
        m_flagReleaseMouse = !m_flagReleaseMouse;
        (*m_sharedContext->window->GetWindow()).setMouseCursorVisible(m_flagReleaseMouse);
        sf::Vector2i windowCenter = sf::Vector2i(m_sharedContext->window->GetWindowSize().x / 2, m_sharedContext->window->GetWindowSize().y / 2);
        sf::Mouse::setPosition(windowCenter, *m_sharedContext->window->GetWindow());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
    {
        m_roll -= m_turnSpeed;
        if (m_roll < -360.0f)
        {
            m_roll += 360.0f;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
    {
        m_roll += m_turnSpeed;
        if (m_roll > 360.0f)
        {
            m_roll -= 360.0f;
        }
    }
}

void Camera::ChangeMovementSpeed(GLfloat delta)
{
    m_movementSpeed += delta;
}

void Camera::Update(GLint l_elapsed)
{
    if (m_cooldown > 0)
    {
        if (m_cooldown - l_elapsed < 0)
        {
            m_cooldown = 0;
        }
        else
        {
            m_cooldown -= l_elapsed;
        }
    }
}
