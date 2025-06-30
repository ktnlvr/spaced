#ifndef __H__ENGINE_INPUT__
#define __H__ENGINE_INPUT__

#include <string.h>

#include <GLFW/glfw3.h>

#include "../vec2i.h"

typedef struct {
  i32 left, right, up, down;
} input_t;

static void input_init(input_t *input) { memset(input, 0, sizeof(input_t)); }

static vec2i input_vector(input_t *input) {
  vec2i ret;
  ret.x = input->right - input->left;
  ret.y = input->up - input->down;
  return ret;
}

#endif
