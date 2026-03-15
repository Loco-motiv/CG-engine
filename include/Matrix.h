#pragma once

#include <MathConstats.h>
#include <cmath>
#include <glad/glad.h>
#include <iostream>

class MatrixFloat
{
public:
    GLfloat m_elements[16];

    MatrixFloat()
    {
        for (GLint i = 0; i < 16; ++i)
        {
            m_elements[i] = 0.0f;
        }
        m_elements[0]  = 1.0f;
        m_elements[5]  = 1.0f;
        m_elements[10] = 1.0f;
        m_elements[15] = 1.0f;
    }

    MatrixFloat(GLfloat m00, GLfloat m01, GLfloat m02, GLfloat m03,
                GLfloat m04, GLfloat m05, GLfloat m06, GLfloat m07,
                GLfloat m08, GLfloat m09, GLfloat m10, GLfloat m11,
                GLfloat m12, GLfloat m13, GLfloat m14, GLfloat m15)
    {
        Set(m00, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15);
    }

    MatrixFloat operator*(const MatrixFloat& n) const
    {
        return MatrixFloat(m_elements[0] * n[0] + m_elements[4] * n[1] + m_elements[8] * n[2] + m_elements[12] * n[3], m_elements[1] * n[0] + m_elements[5] * n[1] + m_elements[9] * n[2] + m_elements[13] * n[3], m_elements[2] * n[0] + m_elements[6] * n[1] + m_elements[10] * n[2] + m_elements[14] * n[3], m_elements[3] * n[0] + m_elements[7] * n[1] + m_elements[11] * n[2] + m_elements[15] * n[3],
                           m_elements[0] * n[4] + m_elements[4] * n[5] + m_elements[8] * n[6] + m_elements[12] * n[7], m_elements[1] * n[4] + m_elements[5] * n[5] + m_elements[9] * n[6] + m_elements[13] * n[7], m_elements[2] * n[4] + m_elements[6] * n[5] + m_elements[10] * n[6] + m_elements[14] * n[7], m_elements[3] * n[4] + m_elements[7] * n[5] + m_elements[11] * n[6] + m_elements[15] * n[7],
                           m_elements[0] * n[8] + m_elements[4] * n[9] + m_elements[8] * n[10] + m_elements[12] * n[11], m_elements[1] * n[8] + m_elements[5] * n[9] + m_elements[9] * n[10] + m_elements[13] * n[11], m_elements[2] * n[8] + m_elements[6] * n[9] + m_elements[10] * n[10] + m_elements[14] * n[11], m_elements[3] * n[8] + m_elements[7] * n[9] + m_elements[11] * n[10] + m_elements[15] * n[11],
                           m_elements[0] * n[12] + m_elements[4] * n[13] + m_elements[8] * n[14] + m_elements[12] * n[15], m_elements[1] * n[12] + m_elements[5] * n[13] + m_elements[9] * n[14] + m_elements[13] * n[15], m_elements[2] * n[12] + m_elements[6] * n[13] + m_elements[10] * n[14] + m_elements[14] * n[15], m_elements[3] * n[12] + m_elements[7] * n[13] + m_elements[11] * n[14] + m_elements[15] * n[15]);
    }

    GLfloat operator[](GLint index) const
    {
        return m_elements[index];
    }

    GLfloat& operator[](GLint index)
    {
        return m_elements[index];
    }

    void Rotate(GLfloat yaw, GLfloat pitch, GLfloat roll)
    {
        yaw   *= DEG2RAD;
        pitch *= DEG2RAD;
        roll  *= DEG2RAD;

        GLfloat yawSin   = std::sin(yaw);
        GLfloat yawCos   = std::cos(yaw);
        GLfloat pitchSin = std::sin(pitch);
        GLfloat pitchCos = std::cos(pitch);
        GLfloat rollSin  = std::sin(roll);
        GLfloat rollCos  = std::cos(roll);

        m_elements[0]  = yawCos * pitchCos;
        m_elements[1]  = yawSin * pitchCos;
        m_elements[2]  = -1 * pitchSin;
        m_elements[4]  = yawCos * pitchSin * rollSin - yawSin * rollCos;
        m_elements[5]  = yawSin * pitchSin * rollSin + yawCos * rollCos;
        m_elements[6]  = pitchCos * rollSin;
        m_elements[8]  = yawCos * pitchSin * rollCos + yawSin * rollSin;
        m_elements[9]  = yawSin * pitchSin * rollCos - yawCos * rollSin;
        m_elements[10] = pitchCos * rollCos;
    }

    void Move(GLfloat xDelta, GLfloat yDelta, GLfloat zDelta)
    {
        m_elements[12] += xDelta;
        m_elements[13] += yDelta;
        m_elements[14] += zDelta;
    }

    void PerspectiveProjection(GLfloat fovX, GLfloat aspectRatio, GLfloat front, GLfloat back)
    {
        GLfloat tangent = tan(fovX / 2.0f * DEG2RAD); // tangent of half fovX

        m_elements[0]  = 1.0f / tangent;
        m_elements[5]  = aspectRatio / tangent;
        m_elements[10] = -(back + front) / (back - front);
        m_elements[11] = -1.0f;
        m_elements[12] = 0.0f;
        m_elements[13] = 0.0f;
        m_elements[14] = -(2.0f * back * front) / (back - front);
    }

    void OrthographicProjection(GLfloat aspectRatio, GLfloat top, GLfloat bottom, GLfloat right, GLfloat left, GLfloat front, GLfloat back)
    {
        m_elements[0]  = 2.0f / (right - left);
        m_elements[5]  = 2.0f * aspectRatio / (top - bottom);
        m_elements[10] = -2.0f / (back - front);
        m_elements[11] = 0.0f;
        m_elements[12] = -(right + left) / (right - left);
        m_elements[13] = -(top + bottom) / (top - bottom);
        m_elements[14] = -(back + front) / (back - front);
    }

    const GLfloat* GetArray() const
    {
        return m_elements;
    }

    void Print() const
    {
        for (GLint row = 0; row < 4; ++row)
        {
            for (GLint col = 0; col < 4; ++col)
            {
                std::cout << m_elements[row * 4 + col] << ' ';
            }
            std::cout << '\n';
        }
    }

    void ScaleX(GLfloat scale)
    {
        m_elements[0] *= scale;
    }

    void ScaleY(GLfloat scale)
    {
        m_elements[5] *= scale;
    }

    void ScaleZ(GLfloat scale)
    {
        m_elements[10] *= scale;
    }

    void ScaleXYZ(GLfloat xScale, GLfloat yScale, GLfloat zScale)
    {
        m_elements[0]  *= xScale;
        m_elements[5]  *= yScale;
        m_elements[10] *= zScale;
    }

    void Set(GLfloat m00, GLfloat m01, GLfloat m02, GLfloat m03,
             GLfloat m04, GLfloat m05, GLfloat m06, GLfloat m07,
             GLfloat m08, GLfloat m09, GLfloat m10, GLfloat m11,
             GLfloat m12, GLfloat m13, GLfloat m14, GLfloat m15)
    {
        m_elements[0]  = m00;
        m_elements[1]  = m01;
        m_elements[2]  = m02;
        m_elements[3]  = m03;
        m_elements[4]  = m04;
        m_elements[5]  = m05;
        m_elements[6]  = m06;
        m_elements[7]  = m07;
        m_elements[8]  = m08;
        m_elements[9]  = m09;
        m_elements[10] = m10;
        m_elements[11] = m11;
        m_elements[12] = m12;
        m_elements[13] = m13;
        m_elements[14] = m14;
        m_elements[15] = m15;
    }
};