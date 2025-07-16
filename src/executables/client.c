#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../setup/client.h"

#include "../6502/ops.h"

int main(void) {
  names_init();

  allocator_t alloc = allocator_new_malloc();

  scheduler_t scheduler;
  scheduler_init(&scheduler, SCHEDULER_STRATEGY_RANDOM, alloc, alloc);

  world_t world;
  world_init(&world);
  resources_add_ty(world_t, &scheduler.resources, world);

  global_assets_t assets;
  global_assets_load(&assets, alloc);

  rendering_ctx_t rendering_ctx;
  rendering_ctx_init(&rendering_ctx);
  rendering_ctx_show_window(&rendering_ctx);
  resources_add_ty(rendering_ctx_t, &scheduler.resources, rendering_ctx);

  gl_quad_init();

  input_t input;
  input_init(&input);
  resources_add_ty(input_t, &scheduler.resources, rendering_ctx);

  entity_t *entt =
      world_spawn_entity_construct(&world, vec2i_zero(), vec2_zero());

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 2; j++) {
      if (i == 0 && (j == 0 || j == -1) || (j - 1 == abs(i)))
        continue;
      block_t mesh = block_new_mesh();
      entity_construct_add_block(entt, vec2i_new(i, j), mesh);
    }
  }

  vec2i chip_offset = vec2i_new(0, 0);
  block_t chip = block_new_chip(alloc, CHIP_MAXIMUM_MEMORY);
  entity_construct_add_block(entt, chip_offset, chip);
  chip.as_chip.chip.userdata = &entt->as_construct;

  vec2i primary_thruster_offset = vec2i_new(0, -1);
  block_t thruster = block_new_thruster(1);
  entity_construct_add_block(entt, primary_thruster_offset, thruster);
  entity_construct_add_block(entt, vec2i_new(-1, 2), thruster);
  entity_construct_add_block(entt, vec2i_new(1, 2), thruster);

  entity_t *camera =
      world_spawn_entity_camera(&world, vec2i_zero(), vec2_zero(), 4.);

  sz thruster_program_size;
  byte *thuster_program = read_binary_file(alloc, "build/thruster/main.bin",
                                           &thruster_program_size);
  chip_load_rom(&chip.as_chip.chip, thuster_program, thruster_program_size,
                0xF800);

  map_insert_ty(vec2i, &entt->as_construct.device_map, 16,
                &primary_thruster_offset);

  double last_t = glfwGetTime();
  while (!rendering_ctx_should_close(&rendering_ctx)) {
    rendering_ctx_frame_begin(&rendering_ctx);

    double t = glfwGetTime();
    double dt = t - last_t;
    last_t = t;

    scheduler_tick(&scheduler, dt);
    scheduler_begin_running(&scheduler);
    scheduler_end_running(&scheduler);

    chip_dbg_dump(&chip.as_chip.chip);
    chip_step(&chip.as_chip.chip);

    rendering_ctx_frame_end(&rendering_ctx);
  }

  rendering_ctx_cleanup(&rendering_ctx);

  world_cleanup(&world);

  names_cleanup();

  return 0;
}
