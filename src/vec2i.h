#ifndef __SPACED_H__VEC2I__
#define __SPACED_H__VEC2I__

#include "defs.h"

typedef struct {
  i32 x, y;
} vec2i;

static vec2i vec2i_new(i32 x, i32 y) {
  vec2i ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

#endif
