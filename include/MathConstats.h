#pragma once

#include <glad/gl.h>
#include <numbers>

constexpr GLfloat PI          = std::numbers::pi;
constexpr GLfloat DEG2RAD     = PI / 180.0f;
constexpr GLfloat DEG2RADHALF = DEG2RAD / 2.0;
constexpr GLfloat RAD2DEG     = 180.0f / PI;