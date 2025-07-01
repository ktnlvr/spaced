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

static void client__schedule_rendering(client_data_t *data) {
  scheduler_declare_system(&data->scheduler, ENTITY_KIND_CAMERA,
                           ENTITY_KIND_MASK_EMPTY, camera_move,
                           SYSTEM_REQ_NO_DEPS,
                           {
                               .phase = SYSTEM_PHASE_PRE_RENDER,
                               .world = (world_t *)1,
                               .input = (input_t *)1,
                           });

  name_t deps_proj[] = {system_camera_move_name};
  scheduler_declare_system(
      &data->scheduler, ENTITY_KIND_CAMERA, ENTITY_KIND_MASK_EMPTY,
      camera_set_projection, deps_proj, 1,
      {
          .phase = SYSTEM_PHASE_PRE_RENDER,
          .system_specific_data = &data->assets.tileset_shader,
          .world = (world_t *)1,
          .input = (input_t *)1,
          .rendering_ctx = (rendering_ctx_t *)1,
      });

  render_constructs_data_t *render_constructs_data = allocator_alloc_ty(
      render_constructs_data_t, data->scheduler.persistent_allocator, 1);

  render_constructs_data->image = &data->assets.tileset_image;
  render_constructs_data->program = data->assets.tileset_shader.gl_program;
  render_constructs_data->vao = _gl_quad_vao;

  name_t deps_render_constructs[] = {system_camera_move_name};
  scheduler_declare_system(&data->scheduler, ENTITY_KIND_CONSTRUCT,
                           ENTITY_KIND_MASK_EMPTY, render_constructs,
                           deps_render_constructs, 1,
                           {
                               .phase = SYSTEM_PHASE_RENDER,
                               .system_specific_data = render_constructs_data,
                               .world = (world_t *)1,
                               .input = (input_t *)1,
                               .rendering_ctx = (rendering_ctx_t *)1,
                           });

  scheduler_declare_system(&data->scheduler, ENTITY_KIND_MASK_EMPTY,
                           ENTITY_KIND_MASK_EMPTY, process_input,
                           SYSTEM_REQ_NO_DEPS,
                           {
                               .phase = SYSTEM_PHASE_PRE_UPDATE,
                               .input = (input_t *)1,
                               .rendering_ctx = (rendering_ctx_t *)1,
                           });
}

static void client_schedule_systems(client_data_t *data) {
  system_req_t reqs = system_req_new();
  reqs.input = &data->input;
  reqs.rendering_ctx = &data->rendering_ctx;
  reqs.world = &data->world;
  scheduler_set_requirements(&data->scheduler, reqs);

  client__schedule_rendering(data);

  FILE *f = fopen("./schedule.tmp.graphviz", "w+");
  scheduler_plan(&data->scheduler);
  scheduler_dump_dependency_graph(&data->scheduler, f);
  fclose(f);
}

#endif
