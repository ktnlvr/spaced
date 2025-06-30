#ifndef __H__SYSTEMS_CAMERA__
#define __H__SYSTEMS_CAMERA__

#include "../rendering/shader.h"
#include "require.h"

#define CAMERA_SPEED 2.5

static void system_camera_move(system_req_t payload,
                               allocator_t temporary_allocator) {
  entity_iter_t it = world_entity_iter(payload.world);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    vec2i inp = input_vector(payload.input);
    vec2 wish =
        vec2_scale(vec2i_to_vec2(inp), CAMERA_SPEED * payload.delta_time);
    ptr->chunk_local_position = vec2_add(ptr->chunk_local_position, wish);
  }
}

static void system_camera_set_projection(system_req_t payload,
                                         allocator_t temporary_allocator) {
  shader_t *shader_ptr = (shader_t *)payload.system_specific_data;
  GLuint program = shader_ptr->gl_program;

  entity_iter_t it = world_entity_iter(payload.world);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    mat4 projection =
        entity_camera_calculate_projection(payload.rendering_ctx, ptr);
    rendering_ctx_set_projection(payload.rendering_ctx, program, projection);
    break;
  }
}

#endif
