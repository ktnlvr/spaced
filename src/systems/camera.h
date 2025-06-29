#ifndef __SPACED_H__SYSTEMS_CAMERA__
#define __SPACED_H__SYSTEMS_CAMERA__

#include "../engine/world.h"

#include "../engine/input.h"

#define CAMERA_SPEED 2.5

static void system_camera_move(world_t *w, float dt, input_t *input) {
  entity_iter_t it = world_entity_iter(w);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    vec2i inp = input_vector(input);
    vec2 wish = vec2_scale(vec2i_to_vec2(inp), CAMERA_SPEED * dt);
    ptr->chunk_local_position = vec2_add(ptr->chunk_local_position, wish);
  }
}

static void system_camera_set_projection(rendering_ctx_t *ctx, world_t *w,
                                         GLuint program) {
  entity_iter_t it = world_entity_iter(w);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    mat4 projection = entity_camera_calculate_projection(ctx, ptr);
    rendering_ctx_set_projection(ctx, program, projection);
    break;
  }
}

#endif
