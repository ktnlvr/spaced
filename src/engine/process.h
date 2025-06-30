#ifndef __H__ENGINE_PROCESS__
#define __H__ENGINE_PROCESS__

#include "../rendering/quads.h"

#include "../rendering/image.h"
#include "entity.h"
#include "world.h"

static void system_render_quads(world_t *world, GLuint program, GLuint vao,
                                image_t *image) {
  glUseProgram(program);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, _gl_quad_vbo);

  image_bind(image);
  GLint sampler_location = glGetUniformLocation(program, "sSampler");
  glUniform1i(sampler_location, 0);

  entity_iter_t it = world_entity_iter_masked(world, ENTITY_KIND_CONSTRUCT);
  while (entity_iter_next(&it)) {
    if (it.entity->as_construct.is_dirty) {
      instance_buffer_clear(&it.entity->as_construct.instance);

      for (int i = 0; i < it.entity->as_construct.components.size; i++) {
        component_t c =
            list_get_ty(component_t, &it.entity->as_construct.components, i);

        instance_t instance;
        instance.position = vec2i_to_vec2(c.offset);
        instance.tile_index = 0;

        instance_buffer_push(&it.entity->as_construct.instance, instance);
      }

      instance_buffer_flush(&it.entity->as_construct.instance);

      it.entity->as_construct.is_dirty = false;
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4,
                          it.entity->as_construct.instance.size);
  }
}

#endif
