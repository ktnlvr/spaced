#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/input.h"
#include "../engine/process.h"
#include "../engine/world.h"
#include "../rendering/context.h"
#include "../rendering/instances.h"
#include "../rendering/quads.h"
#include "../systems/camera.h"
#include "../systems/scheduler.h"

const char *vertex_src = "#version 330 core\n"
                         "layout (location = 0) in vec2 aPos;"
                         "layout (location = 1) in uint tileIndex;"
                         "layout (location = 2) in vec2 aOffset;"
                         "uniform mat4 uProjection;"
                         "void main() {"
                         "    gl_Position = uProjection * vec4(aPos + aOffset "
                         "+ float(tileIndex), 0.0, 1.0);"
                         "}";

const char *fragment_src = "#version 330 core\n"
                           "out vec4 FragColor;"
                           "void main() {"
                           "    FragColor = vec4(0.4, 0.7, 1.0, 1.0);"
                           "}";

int main(void) {
  allocator_t alloc = allocator_new_malloc();

  rendering_ctx_t ctx;
  rendering_ctx_init(&ctx);

  world_t world;
  world_init(&world);

  input_t input;
  input_init(&input);

  gl_quad_init();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_src, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_src, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  scheduler_t scheduler =
      scheduler_new(SCHEDULER_STRATEGY_RANDOM, allocator_new_malloc(),
                    allocator_new_malloc());

  system_req_t camera_move_req = system_req_new();
  camera_move_req.input = SYSTEM_REQ_PTR_REQUIRED;
  camera_move_req.world = SYSTEM_REQ_PTR_REQUIRED;
  scheduler_add_system(&scheduler, camera_move_req, system_camera_move);

  system_req_t camera_set_projection_req = system_req_new();
  camera_set_projection_req.input = SYSTEM_REQ_PTR_REQUIRED;
  camera_set_projection_req.world = SYSTEM_REQ_PTR_REQUIRED;
  camera_set_projection_req.rendering_ctx = SYSTEM_REQ_PTR_REQUIRED;
  camera_set_projection_req.system_specific_data = &program;
  scheduler_add_system(&scheduler, camera_set_projection_req,
                       system_camera_set_projection);

  scheduler_plan(&scheduler);

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

    system_render_quads(&world, program, _gl_quad_vao);
    scheduler_tick(&scheduler, dt);
    scheduler_begin_running(&scheduler);
    scheduler_end_running(&scheduler);

    rendering_ctx_frame_end(&ctx);
  }

  rendering_ctx_cleanup(&ctx);

  world_cleanup(&world);

  return 0;
}
