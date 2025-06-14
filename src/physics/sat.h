#ifndef __SPACED_H__SAT__
#define __SPACED_H__SAT__

#include "../vec2.h"

typedef vec2 float2;

static bool sat_float2_overlap(float2 a, float2 b) {
  return a.y > b.x && b.y > a.x;
}

static float2 sat_vec2_project(vec2 *vs, sz n, vec2 axis) {
  float2 ret;

  // TODO: replace builtins
  ret.x = __builtin_inff();
  ret.y = -__builtin_inff();

  for (sz i = 0; i < n; i++) {
    float proj = vec2_dot(vs[i], axis);
    if (proj < ret.x)
      ret.x = proj;
    if (proj > ret.y)
      ret.y = proj;
  }

  return ret;
}

static bool sat_polygons(vec2 *vs1, sz n1, vec2 *vs2, sz n2) {
  // TODO: we assume that having the same axes is basically impossible
  vec2 *axes = (vec2 *)calloc(sizeof(vec2), n1 + n2);

  for (int i = 0; i < n1; i++) {
    vec2 s = vec2_sub(vs1[(i + 1) % n1], vs1[i]);
    vec2 p = vec2_perpendicular(s);
    axes[i] = p;
  }

  for (int i = 0; i < n2; i++) {
    vec2 s = vec2_sub(vs2[(i + 1) % n2], vs2[i]);
    vec2 p = vec2_perpendicular(s);
    axes[i + n1] = p;
  }

  for (sz i = 0; i < n1 + n2; i++) {
    float2 a = sat_vec2_project(vs1, n1, axes[i]);
    float2 b = sat_vec2_project(vs2, n2, axes[i]);
    if (!sat_float2_overlap(a, b))
      return false;
  }

  return true;
}

#endif
