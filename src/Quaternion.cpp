#include "Quaternion.h"

Quaternion::Quaternion(sf::Vector3f xyz, GLfloat l_angle)
{
    l_angle      *= DEG2RADHALF; //* angle in degree
    GLfloat sine  = sinf(l_angle);
    s             = cosf(l_angle);
    x             = xyz.x * sine;
    y             = xyz.y * sine;
    z             = xyz.z * sine;
}

Quaternion::Quaternion(sf::Vector3f l_angles)
{
    Quaternion qx, qy, qz;

    l_angles *= DEG2RADHALF;

    qx.x = sinf(l_angles.x);
    qx.s = cosf(l_angles.x);
    qy.y = sinf(l_angles.y);
    qy.s = cosf(l_angles.y);
    qz.z = sinf(l_angles.z);
    qz.s = cosf(l_angles.z);

    qx = qx * qy * qz;

    this->s = qx.s;
    this->x = qx.x;
    this->y = qx.y;
    this->z = qx.z;
}

Quaternion::Quaternion(GLfloat l_s, GLfloat l_x, GLfloat l_y, GLfloat l_z)
{
    s = l_s;
    x = l_x;
    y = l_y;
    z = l_z;
}

void Quaternion::Normalize()
{
    GLfloat length        = sqrtf(s * s + x * x + y * y + z * z);
    const GLfloat EPSILON = 1e-6f;
    if (length > EPSILON)
    {
        GLfloat invLength  = 1.0f / length;
        s                 *= invLength;
        x                 *= invLength;
        y                 *= invLength;
        z                 *= invLength;
    }
}

MatrixFloat Quaternion::GetMatrix() const
{
    GLfloat x2  = x + x;
    GLfloat y2  = y + y;
    GLfloat z2  = z + z;
    GLfloat xx2 = x * x2;
    GLfloat xy2 = x * y2;
    GLfloat xz2 = x * z2;
    GLfloat yy2 = y * y2;
    GLfloat yz2 = y * z2;
    GLfloat zz2 = z * z2;
    GLfloat sx2 = s * x2;
    GLfloat sy2 = s * y2;
    GLfloat sz2 = s * z2;

    MatrixFloat result;

    result.Set(1 - (yy2 + zz2), xy2 + sz2, xz2 - sy2, 0,
               xy2 - sz2, 1 - (xx2 + zz2), yz2 + sx2, 0,
               xz2 + sy2, yz2 - sx2, 1 - (xx2 + yy2), 0,
               0, 0, 0, 1);

    return result;
}

sf::Vector3f Quaternion::GetEulerAngles() const
{
    sf::Vector3f euler;

    GLfloat sinr_cosp = 2.0f * (s * x + y * z);
    GLfloat cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    euler.x           = std::atan2(sinr_cosp, cosr_cosp);

    GLfloat sinp = 2.0f * (s * y - x * z);
    euler.y      = -std::asin(sinp);

    GLfloat siny_cosp = 2.0f * (s * z + x * y);
    GLfloat cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    euler.z           = std::atan2(siny_cosp, cosy_cosp);

    euler.x *= RAD2DEG;
    euler.y *= RAD2DEG;
    euler.z *= RAD2DEG;

    return euler;
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