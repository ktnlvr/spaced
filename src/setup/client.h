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
#include "../systems/camera.h"
#include "../systems/construct.h"
#include "../systems/input.h"
#include "../systems/physics.h"
#include "../systems/require.h"
#include "../systems/scheduler.h"

typedef struct {
  shader_t tileset_shader;
  image_t tileset_image;
} global_assets_t;

typedef struct {
  allocator_t alloc;
  world_t world;
  rendering_ctx_t rendering_ctx;
  input_t input;
  scheduler_t scheduler;

  global_assets_t assets;
} client_data_t;

static void client_data__load_assets(client_data_t *data) {
  sz file_size;
  void *file_buffer =
      read_binary_file(data->alloc, "./assets/tileset.png", &file_size);

  image_init(&data->assets.tileset_image, data->alloc, file_buffer, file_size);
  allocator_free(data->alloc, file_buffer);
  image_bind(&data->assets.tileset_image);

  char *vertex_source = read_text_file(data->alloc, "./assets/tileset.vert");
  char *fragment_source = read_text_file(data->alloc, "./assets/tileset.frag");

  shader_init(&data->assets.tileset_shader, vertex_source, fragment_source);
}

static void client_data_init(client_data_t *data, allocator_t alloc) {
  names_init();

  data->alloc = alloc;

  world_init(&data->world);

  rendering_ctx_init(&data->rendering_ctx);
  rendering_ctx_show_window(&data->rendering_ctx);
  gl_quad_init();

  input_init(&data->input);

  data->scheduler = scheduler_new(SCHEDULER_STRATEGY_RANDOM, alloc, alloc);

  client_data__load_assets(data);
}

static void client__schedule_physics(client_data_t *data) {
  scheduler_t *s = &data->scheduler;

  scheduler_add_system_declared(s, &system_decl_integrate_velocity);

  scheduler_add_system_declared(s, &system_decl_tick_thrusters);
}

static void client__schedule_rendering(client_data_t *data) {
  scheduler_t *s = &data->scheduler;

  scheduler_add_system_declared(s, &system_decl_camera_move);

  render_constructs_data_t *render_constructs_data = allocator_alloc_ty(
      render_constructs_data_t, data->scheduler.persistent_allocator, 1);

  render_constructs_data->image = &data->assets.tileset_image;
  render_constructs_data->program = data->assets.tileset_shader.gl_program;
  render_constructs_data->vao = _gl_quad_vao;
  scheduler_add_system_declared_specific_data(s, &system_decl_render_constructs,
                                              render_constructs_data);

  scheduler_add_system_declared(s, &system_decl_process_input);
}

static void client_schedule_systems(client_data_t *data) {
  system_payload_t parent_payload = system_payload_new(
      &data->world, &data->input, &data->rendering_ctx, 0., 0.);

  scheduler_set_parent_payload(&data->scheduler, parent_payload);

  client__schedule_physics(data);
  client__schedule_rendering(data);

  FILE *f = fopen("./schedule.tmp.graphviz", "w+");
  scheduler_dump_dependency_graph(&data->scheduler, f);
  fclose(f);
}

#endif
