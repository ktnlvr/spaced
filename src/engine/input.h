#ifndef __SPACED_H__ENGINE_INPUT__
#define __SPACED_H__ENGINE_INPUT__

#include <GLFW/glfw3.h>

#include "../defs.h"
#include <string.h>

typedef struct {
  i32 left, right, up, down;
} input_t;

static void input_init(input_t *input) { memset(input, 0, sizeof(input_t)); }

static void input_tick(GLFWwindow *window, input_t *input) {
  int keys[] = {GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_S, GLFW_KEY_W};
  int *outs[] = {&input->left, &input->right, &input->down, &input->up};

  for (int i = 0; i < 4; i++) {
    int k = keys[i];
    int state = glfwGetKey(window, k);

    switch (state) {
    case GLFW_PRESS:
      *outs[i] = 1;
      break;
    case GLFW_RELEASE:
      *outs[i] = 0;
      break;
    }
  }
}

#endif
