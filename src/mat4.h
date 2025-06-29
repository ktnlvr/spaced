#ifndef __SPACED_H__MAT4__
#define __SPACED_H__MAT4__

#include <string.h>

// A column-major 4D matrix
typedef struct {
  float data[16];
} mat4;

static mat4 mat4_zero() {
  mat4 ret;
  memset(&ret.data, 0, sizeof(float) * 16);
  return ret;
}

#endif
