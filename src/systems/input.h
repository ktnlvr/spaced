#ifndef __H__SYSTEMS_INPUT__
#define __H__SYSTEMS_INPUT__

#include "require.h"

#include "../engine/input.h"

static void system_process_input(system_req_t payload,
                                        allocator_t temporary_allocator) {
  input_t *input = payload.input;
  GLFWwindow *window = payload.rendering_ctx->window;

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
