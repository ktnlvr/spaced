#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../setup/client.h"

int main(void) {
  allocator_t alloc = allocator_new_malloc();
  client_data_t my_data;
  client_data_init(&my_data, alloc);
  client_schedule_systems(&my_data);

  entity_t *entt =
      world_spawn_entity_construct(&my_data.world, vec2i_zero(), vec2_zero());

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 2; j++) {
      if (i == 0 && (j == 0 || j == 1))
        continue;
      block_t mesh = block_new_mesh();
      entity_construct_add_block(entt, vec2i_new(i, j), mesh);
    }
  }

  block_t chip = block_new_chip(alloc, CHIP_MAXIMUM_MEMORY);
  entity_construct_add_block(entt, vec2i_new(0, 0), chip);

  entity_t *camera =
      world_spawn_entity_camera(&my_data.world, vec2i_zero(), vec2_zero(), 4.);

  double last_t = glfwGetTime();
  while (!rendering_ctx_should_close(&my_data.rendering_ctx)) {
    rendering_ctx_frame_begin(&my_data.rendering_ctx);

    double t = glfwGetTime();
    double dt = t - last_t;
    last_t = t;

    scheduler_tick(&my_data.scheduler, dt);
    scheduler_begin_running(&my_data.scheduler);
    scheduler_end_running(&my_data.scheduler);

    rendering_ctx_frame_end(&my_data.rendering_ctx);
  }

  rendering_ctx_cleanup(&my_data.rendering_ctx);

  world_cleanup(&my_data.world);

  names_cleanup();

  return 0;
}
