#ifndef __SPACED_H__VEC2I__
#define __SPACED_H__VEC2I__

#include "defs.h"
#include "vec2.h"

typedef struct {
  i32 x, y;
} vec2i;

static vec2i vec2i_new(i32 x, i32 y) {
  vec2i ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

static vec2i vec2i_zero() { return vec2i_new(0, 0); }

static vec2 vec2i_to_vec2(vec2i v) {
  vec2 ret;
  ret.x = v.x;
  ret.y = v.y;
  return ret;
}

#endif
