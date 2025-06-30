#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../engine/input.h"
#include "../engine/process.h"
#include "../engine/world.h"
#include "../misc.h"
#include "../rendering/context.h"
#include "../rendering/image.h"
#include "../rendering/instances.h"
#include "../rendering/quads.h"
#include "../rendering/shader.h"
#include "../systems/camera.h"
#include "../systems/construct.h"
#include "../systems/scheduler.h"

const char *vertex_src =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;"
    "layout (location = 1) in vec2 texCoord;"
    "layout (location = 2) in uint tileIndex;"
    "layout (location = 3) in vec2 aOffset;"
    "out vec2 vTexCoord;"
    "uniform mat4 uProjection;"
    "uniform ivec2 tilemapTotal;"
    "uniform ivec2 tileSize;"
    "void main() {"
    "    gl_Position = uProjection * vec4(aPos + aOffset, 0.0, 1.0);"
    "    vec2 atlasUV = vec2(tilemapTotal);"
    "    vec2 singleTile = vec2(tileSize) / atlasUV;"
    "    uint col = tileIndex % uint(tilemapTotal.x);"
    "    uint row = tileIndex / uint(tilemapTotal.x);"
    "    vec2 cellOffset = vec2(col, row) * singleTile;"
    "    vTexCoord = cellOffset + texCoord * singleTile;"
    "}";

const char *fragment_src = "#version 330 core\n"
                           "out vec4 FragColor;"
                           "uniform sampler2D sSampler;"
                           "in vec2 vTexCoord;"
                           "void main() {"
                           "    vec4 tex = texture(sSampler, vTexCoord);"
                           "    FragColor = tex;"
                           "}";

int main(void) {
  names_init();

  allocator_t alloc = allocator_new_malloc();

  rendering_ctx_t ctx;
  rendering_ctx_init(&ctx);
  rendering_ctx_show_window(&ctx);

  world_t world;
  world_init(&world);

  input_t input;
  input_init(&input);

  gl_quad_init();

  // Load the tileset
  sz file_size;
  void *file_buffer =
      read_binary_file(alloc, "./assets/tileset.png", &file_size);

  image_t tileset;
  image_init(&tileset, alloc, file_buffer, file_size);
  allocator_free(alloc, file_buffer);

  image_bind(&tileset);

  shader_t shader;
  shader_init(&shader, vertex_src, fragment_src);

  scheduler_t scheduler =
      scheduler_new(SCHEDULER_STRATEGY_RANDOM, allocator_new_malloc(),
                    allocator_new_malloc());

  scheduler_declare_system(&scheduler, ENTITY_KIND_CAMERA,
                           ENTITY_KIND_MASK_EMPTY, camera_move,
                           SYSTEM_REQ_NO_DEPS, 0,
                           {
                               .input = SYSTEM_REQ_PTR_REQUIRED,
                               .world = SYSTEM_REQ_PTR_REQUIRED,
                           });

  name_t deps_proj[] = {system_camera_move_name};
  scheduler_declare_system(&scheduler, ENTITY_KIND_CAMERA,
                           ENTITY_KIND_MASK_EMPTY, camera_set_projection,
                           deps_proj, 1,
                           {
                               .input = SYSTEM_REQ_PTR_REQUIRED,
                               .world = SYSTEM_REQ_PTR_REQUIRED,
                               .rendering_ctx = SYSTEM_REQ_PTR_REQUIRED,
                               .system_specific_data = &shader,
                           });

  render_constructs_data_t render_constructs_data;
  render_constructs_data.image = &tileset;
  render_constructs_data.program = shader.gl_program;
  render_constructs_data.vao = _gl_quad_vao;

  name_t deps_render_constructs[] = {system_camera_move_name};
  scheduler_declare_system(&scheduler, ENTITY_KIND_CONSTRUCT,
                           ENTITY_KIND_MASK_EMPTY, render_constructs,
                           deps_render_constructs, 1,
                           {
                               .input = SYSTEM_REQ_PTR_REQUIRED,
                               .world = SYSTEM_REQ_PTR_REQUIRED,
                               .rendering_ctx = SYSTEM_REQ_PTR_REQUIRED,
                               .system_specific_data = &render_constructs_data,
                           });

  FILE *f = fopen("./schedule.tmp.graphviz", "w+");
  scheduler_plan(&scheduler);
  scheduler_dump_dependency_graph(&scheduler, f);
  fclose(f);

  system_req_t reqs = system_req_new();
  reqs.input = &input;
  reqs.rendering_ctx = &ctx;
  reqs.world = &world;
  scheduler_set_requirements(&scheduler, reqs);

  entity_t *entt =
      world_spawn_entity_construct(&world, vec2i_zero(), vec2_zero());

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      component_t mesh = component_new_mesh();
      entity_construct_add_component(entt, vec2i_new(i, j), mesh);
    }
  }

  entity_t *camera =
      world_spawn_entity_camera(&world, vec2i_zero(), vec2_zero(), 4.);

  double last_t = glfwGetTime();
  while (!rendering_ctx_should_close(&ctx)) {
    rendering_ctx_frame_begin(&ctx);

    double t = glfwGetTime();
    double dt = t - last_t;
    last_t = t;

    input_tick(ctx.window, &input);

    scheduler_tick(&scheduler, dt);
    scheduler_begin_running(&scheduler);
    scheduler_end_running(&scheduler);

    rendering_ctx_frame_end(&ctx);
  }

  rendering_ctx_cleanup(&ctx);

  world_cleanup(&world);

  names_cleanup();

  return 0;
}
