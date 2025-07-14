#ifndef __H__SYSTEMS_CONSTRUCT__
#define __H__SYSTEMS_CONSTRUCT__

#include "../schedule/requirements.h"

#include "../rendering/image.h"
#include "../rendering/quads.h"

#define THRUSTER_POWER 0.01

typedef struct {
  GLuint program;
  GLuint vao;
  image_t *image;
} render_constructs_data_t;

static void system_render_constructs(system_payload_t payload,
                                     allocator_t temporary_allocator) {
  render_constructs_data_t *ptr =
      resources_get_ty(render_constructs_data_t, payload.resources);
  world_t *world = resources_get_world(payload.resources);

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

  entity_iter_t it = world_entity_iter_masked(world, ENTITY_KIND_CONSTRUCT);
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

static const char *system_decl_render_constructs_deps[] = {
    "system_camera_move"};

static system_requirements_declaration_t system_decl_render_constructs = {
    .name = "system_render_constructs",
    .phase = SYSTEM_PHASE_RENDER,
    .entities_const = 0,
    .entities_mut = ENTITY_KIND_CONSTRUCT,
    .dependencies = system_decl_render_constructs_deps,
    .dependency_count = 1,
    .pin_to_main = false,
    .runner = system_render_constructs};

static void system_tick_thrusters(system_req_t payload, allocator_t _) {
  entity_iter_t it = system_payload_entity_iter(&payload);
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

static system_requirements_declaration_t system_decl_tick_thrusters = {
    .name = "system_tick_thrusters",
    .phase = SYSTEM_PHASE_FIXED_UPDATE,
    .entities_const = 0,
    .entities_mut = ENTITY_KIND_CONSTRUCT,
    .dependencies = 0,
    .dependency_count = 0,
    .pin_to_main = false,
    .runner = system_render_constructs};

#endif
