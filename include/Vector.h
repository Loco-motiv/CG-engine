#pragma once

#include <SFML/Window.hpp>

inline float Dot(const sf::Vector3f& lhs, const sf::Vector3f& rhs)
{
    return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

inline sf::Vector3f Cross(const sf::Vector3f& lhs, const sf::Vector3f& rhs)
{
    return sf::Vector3f(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

inline void Normalize(sf::Vector3f& vector)
{
    float xxyyzz = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;

    float invLength  = 1.0f / sqrtf(xxyyzz);
    vector.x        *= invLength;
    vector.y        *= invLength;
    vector.z        *= invLength;
    return;
}