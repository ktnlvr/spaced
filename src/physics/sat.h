#ifndef __SPACED_H__SAT__
#define __SPACED_H__SAT__

#include "../vec2.h"
#include <alloca.h>

typedef vec2 float2;

static bool sat_float2_is_overlap(float2 a, float2 b) {
  return a.y > b.x && b.y > a.x;
}

static float2 sat_float2_overlap(float2 a, float2 b) {
  float2 ret;
  ret.x = (a.x > b.x) ? a.x : b.x;
  ret.y = (a.y > b.y) ? a.y : b.y;
  return ret;
}

static float2 sat_vec2_project(vec2 *vs, sz n, vec2 axis) {
  float2 ret;

  ret.x = FLOAT32_INFTY;
  ret.y = FLOAT32_NEG_INFTY;

  for (sz i = 0; i < n; i++) {
    float proj = vec2_dot(vs[i], axis);
    if (proj < ret.x)
      ret.x = proj;
    if (proj > ret.y)
      ret.y = proj;
  }

  return ret;
}

static bool sat_intersect(vec2 *vs1, sz n1, vec2 *vs2, sz n2, float2 *minimal) {
  if (minimal) {
    minimal->x = FLOAT32_INFTY;
    minimal->y = FLOAT32_NEG_INFTY;
  }

  // TODO: we assume that having the same axes is basically impossible
  norm2 *axes = (norm2 *)alloca(sizeof(vec2) * (n1 + n2));

  for (int i = 0; i < n1; i++) {
    vec2 s = vec2_sub(vs1[(i + 1) % n1], vs1[i]);
    vec2 p = vec2_perpendicular(s);
    norm2 pn = vec2_normalize(p);
    axes[i] = pn;
  }

  for (int i = 0; i < n2; i++) {
    vec2 s = vec2_sub(vs2[(i + 1) % n2], vs2[i]);
    vec2 p = vec2_perpendicular(s);
    norm2 pn = vec2_normalize(p);
    axes[i + n1] = pn;
  }

  bool is_overlap = false;
  for (sz i = 0; i < n1 + n2; i++) {
    norm2 axis = axes[i];
    float2 a = sat_vec2_project(vs1, n1, axis);
    float2 b = sat_vec2_project(vs2, n2, axis);
    is_overlap |= sat_float2_is_overlap(a, b);

    if (minimal) {
      float2 overlap = sat_float2_overlap(a, b);
      // NOTE(Artur): fabsf needed because the initial sentinel is 
      // two infinities the other way around
      if (overlap.y - overlap.x < fabsf(minimal->y - minimal->x))
        *minimal = overlap;
    } else if (is_overlap) {
      return true;
    }
  }

  return is_overlap;
}

#endif
