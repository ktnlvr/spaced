#ifndef __SPACED_H__ENGINE_PROCESS__
#define __SPACED_H__ENGINE_PROCESS__

#include "construct.h"
#include "world.h"

static void system_render_quads(world_t *world, GLuint program, GLuint vao) {
  glUseProgram(program);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, _gl_quad_vbo);

  for (entity_ptr_t entt = world->entities; entt; entt = entt->next) {
    if (entt->kind != ENTITY_KIND_RENDER_QUADS)
      continue;

    render_quads_t *quads = entity_ptr_as_render_quads(entt);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, quads->instances.size);
  }
}

#endif
