#ifndef __H__VEC2__
#define __H__VEC2__

#include "defs.h"

#include <math.h>

/// @brief The vectors of this length and smaller are considered 0.
#define VEC2_EPSILON 1e-7

typedef struct {
  float x, y;
} vec2;

/// @brief Normalized vector. Does not do any work to uphold a variants,
/// just is more type-safe. If a function returns a norm2 it must be
/// normalized.
typedef vec2 norm2;

/// @brief `{.x = x, .y = y}`
static vec2 vec2_new(float x, float y) {
  vec2 ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

/// @brief `{.x = 0, .y = 0}`
static vec2 vec2_zero() { return vec2_new(0., 0.); }

/// @brief `{.x = a.x + b.x, .y = a.y + b.y}`
/// @memberof vec2
static vec2 vec2_add(vec2 a, vec2 b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

/// @brief `{.x = a.x - b.x, .y = a.y - b.y}`
/// @memberof vec2
static vec2 vec2_sub(vec2 a, vec2 b) {
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

/// @brief `{.x = c * a.x, .y = c * a.y}`
/// @memberof vec2
static vec2 vec2_scale(vec2 a, float c) {
  a.x *= c;
  a.y *= c;
  return a;
}

/// @brief `{.x = -y, .y = x}`
/// @memberof vec2
static vec2 vec2_perpendicular(vec2 v) {
  vec2 ret;
  ret.x = -v.y;
  ret.y = v.x;
  return ret;
}

/// @brief Returns a vector of length 1. Returns the 0 vector for 
/// vectors of length @ref VEC2_EPSILON and smaller.
/// @memberof vec2
static norm2 vec2_normalize(vec2 v) {
  norm2 ret;
  ret.x = 0.;
  ret.y = 0.;

  float l = sqrtf(v.x * v.x + v.y * v.y);
  if (l < VEC2_EPSILON)
    return ret;

  ret.x = v.x / l;
  ret.y = v.y / l;
  return ret;
}

/// @brief Returns a scalar product of the two vectors.
/// @memberof vec2
static float vec2_dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

#endif
