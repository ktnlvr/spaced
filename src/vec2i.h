#ifndef __H__VEC2I__
#define __H__VEC2I__

#include "defs.h"
#include "vec2.h"

typedef struct {
  i32 x, y;
} vec2i;

/// @brief `{.x = x, .y = y}`
static vec2i vec2i_new(i32 x, i32 y) {
  vec2i ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

/// @brief `{.x = 0, .y = 0}`
static vec2i vec2i_zero() { return vec2i_new(0, 0); }

/// @brief `{.x = (int)v.x, .y = (int)v.y)`
static vec2 vec2i_to_vec2(vec2i v) {
  vec2 ret;
  ret.x = v.x;
  ret.y = v.y;
  return ret;
}

static bool vec2_eq(vec2i a, vec2i b) { return a.x == b.x && a.y == b.y; }

#endif
