#ifndef __H__SYSTEMS_CONSTRUCT__
#define __H__SYSTEMS_CONSTRUCT__

#include "require.h"

#include "../rendering/image.h"
#include "../rendering/quads.h"

#define THRUSTER_POWER 0.01

typedef struct {
  GLuint program;
  GLuint vao;
  image_t *image;
} render_constructs_data_t;

static void system_render_constructs(system_req_t payload,
                                     allocator_t temporary_allocator) {
  render_constructs_data_t *ptr =
      (render_constructs_data_t *)payload.system_specific_data;

  glUseProgram(ptr->program);
  glBindVertexArray(ptr->vao);
  glBindBuffer(GL_ARRAY_BUFFER, _gl_quad_vbo);

  image_bind(ptr->image);
  GLint sampler_location = glGetUniformLocation(ptr->program, "sSampler");
  glUniform1i(sampler_location, 0);
  GLint tilemap_size = glGetUniformLocation(ptr->program, "tilemapSizePixels");
  glUniform2i(tilemap_size, 128, 128);
  GLint tilemap_tile_size =
      glGetUniformLocation(ptr->program, "tileSizePixels");
  glUniform2i(tilemap_tile_size, 16, 16);

  entity_iter_t it =
      world_entity_iter_masked(payload.world, ENTITY_KIND_CONSTRUCT);
  while (entity_iter_next(&it)) {
    if (it.entity->as_construct.is_dirty) {
      instance_buffer_clear(&it.entity->as_construct.instance);

      for (int i = 0; i < it.entity->as_construct.blocks.size; i++) {
        block_t b = list_get_ty(block_t, &it.entity->as_construct.blocks, i);

        instance_t instance;
        instance.position = vec2i_to_vec2(b.offset);
        instance.tile_index = block_kind_to_tile_index(b.kind);

        instance_buffer_push(&it.entity->as_construct.instance, instance);
      }

      instance_buffer_flush(&it.entity->as_construct.instance);

      it.entity->as_construct.is_dirty = false;
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4,
                          it.entity->as_construct.instance.size);
  }
}

static void system_tick_thrusters(system_req_t payload, allocator_t _) {
  entity_iter_t it = system_req_entity_iter(&payload);
  while (entity_iter_next(&it)) {
    ASSERT__(it.entity->kind == ENTITY_KIND_CONSTRUCT);

    for (sz i = 0; i < it.entity->as_construct.blocks.size; i++) {
      block_t *blk =
          list_get_ty_ptr(block_t, &it.entity->as_construct.blocks, i);
      if (blk->kind != BLOCK_KIND_THRUSTER)
        continue;
      if (blk->as_thruster.throttle == 0)
        continue;

      blk->as_thruster.throttle--;

      float acceleration =
          THRUSTER_POWER * (float)blk->as_thruster.throttle / 255.f;
      vec2 *v = &it.entity->as_construct.velocity;
      *v = vec2_add(*v, vec2_new(0, acceleration));
    }
  }
}

#endif
