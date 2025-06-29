#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/input.h"
#include "../engine/world.h"
#include "../engine/process.h"
#include "../rendering/context.h"
#include "../rendering/quads.h"
#include "../rendering/instances.h"

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

  entity_t *entt =
      world_spawn_entity_construct(&world, vec2i_zero(), vec2_zero());

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      component_t mesh = component_new_mesh();
      entity_construct_add_component(entt, vec2i_new(i, j), mesh);
    }
  }

  while (!rendering_ctx_should_close(&ctx)) {
    rendering_ctx_frame_begin(&ctx);
    rendering_ctx_set_projection(&ctx, program, 7., vec2_zero());

    system_render_quads(&world, program, _gl_quad_vao);

    input_tick(ctx.window, &input);

    rendering_ctx_frame_end(&ctx);
  }

  rendering_ctx_cleanup(&ctx);

  world_cleanup(&world);

  return 0;
}
