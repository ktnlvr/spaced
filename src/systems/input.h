#ifndef __H__SYSTEMS_INPUT__
#define __H__SYSTEMS_INPUT__

#include "../schedule/requirements.h"

#include "../engine/input.h"

static void system_process_input(system_payload_t payload,
                                 allocator_t temporary_allocator) {
  input_t *input = resources_get_ty(input_t, payload.resources);
  GLFWwindow *window =
      resources_get_ty(rendering_ctx_t, payload.resources)->window;

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

static system_requirements_declaration_t system_decl_process_input = {
    .name = "system_process_input",
    .dependencies = 0,
    .dependency_count = 0,
    .entities_const = 0,
    .entities_mut = 0,
    .phase = SYSTEM_PHASE_PRE_UPDATE,
    .pin_to_main = false,
    .runner = system_process_input};

#endif
