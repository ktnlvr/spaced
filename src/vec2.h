#ifndef __SPACED_H__VEC2__
#define __SPACED_H__VEC2__

#include "defs.h"

#include <math.h>

#define VEC2_EPSILON 1e-7

typedef struct {
  float x, y;
} vec2;

typedef vec2 norm2;

static vec2 vec2_new(float x, float y) {
  vec2 ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

static vec2 vec2_zero() {
  return vec2_new(0., 0.);
}

static vec2 vec2_sub(vec2 a, vec2 b) {
  vec2 ret;
  ret.x = a.x - b.x;
  ret.y = a.y - b.y;
  return ret;
}

static vec2 vec2_scale(vec2 a, float c) {
  a.x *= c;
  a.y *= c;
  return a;
}

static vec2 vec2_perpendicular(vec2 v) {
  vec2 ret;
  ret.x = -v.y;
  ret.y = v.x;
  return ret;
}

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

static float vec2_dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

#endif
