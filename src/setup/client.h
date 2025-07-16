#ifndef __H__SETUP_CLIENT__
#define __H__SETUP_CLIENT__

#include "../engine/world.h"

#include "../engine/input.h"
#include "../memory.h"
#include "../misc.h"
#include "../rendering/context.h"
#include "../rendering/image.h"
#include "../rendering/quads.h"
#include "../rendering/shader.h"
#include "../schedule/scheduler.h"
#include "../systems/camera.h"
#include "../systems/construct.h"
#include "../systems/input.h"
#include "../systems/physics.h"

typedef struct {
  shader_t tileset_shader;
  image_t tileset_image;
} global_assets_t;

static void global_assets_load(global_assets_t *assets, allocator_t alloc) {
  sz file_size;

  /// XXX: leak
  void *file_buffer =
      read_binary_file(alloc, "./assets/tileset.png", &file_size);

  image_init(&assets->tileset_image, alloc, file_buffer, file_size);
  image_bind(&assets->tileset_image);

  char *vertex_source = read_text_file(alloc, "./assets/tileset.vert");
  char *fragment_source = read_text_file(alloc, "./assets/tileset.frag");

  shader_init(&assets->tileset_shader, vertex_source, fragment_source);
}

static void client__schedule_physics(scheduler_t *s) {
  scheduler_add_system_declared(s, &system_decl_integrate_velocity);
  scheduler_add_system_declared(s, &system_decl_tick_thrusters);
}

static void client__schedule_rendering(global_assets_t *assets,
                                       scheduler_t *s) {
  scheduler_add_system_declared(s, &system_decl_camera_move);

  render_constructs_data_t *render_constructs_data =
      allocator_alloc_ty(render_constructs_data_t, s->persistent_allocator, 1);

  render_constructs_data->image = &assets->tileset_image;
  render_constructs_data->program = assets->tileset_shader.gl_program;
  render_constructs_data->vao = _gl_quad_vao;
  scheduler_add_system_declared_specific_data(s, &system_decl_render_constructs,
                                              render_constructs_data);

  scheduler_add_system_declared(s, &system_decl_process_input);
}

static void client_schedule_systems(global_assets_t *assets,
                                    scheduler_t *scheduler) {
  client__schedule_physics(scheduler);
  client__schedule_rendering(assets, scheduler);

  FILE *f = fopen("./schedule.tmp.graphviz", "w+");
  scheduler_dump_dependency_graph(&scheduler, f);
  fclose(f);
}

#endif
