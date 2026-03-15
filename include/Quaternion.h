#pragma once

#include "Matrix.h"
#include "Vector.h"

#include <SFML/Window.hpp>

struct Quaternion
{
    GLfloat s; //* scalar part, s
    GLfloat x; //* vector part (x, y, z)
    GLfloat y;
    GLfloat z;

    Quaternion() : s(1), x(0), y(0), z(0) {}
    Quaternion(sf::Vector3f xyz, GLfloat l_angle);
    Quaternion(sf::Vector3f l_angles); //* angles around world axis
    Quaternion(GLfloat s, GLfloat x, GLfloat y, GLfloat z);

    void Normalize();

    MatrixFloat GetMatrix() const; //* return as 4x4 matrix
    sf::Vector3f GetEulerAngles() const;

    Quaternion operator*(const Quaternion& rhs) const;
};