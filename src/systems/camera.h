#ifndef __H__SYSTEMS_CAMERA__
#define __H__SYSTEMS_CAMERA__

#include "../rendering/shader.h"
#include "../schedule/requirements.h"

#define CAMERA_SPEED 2.5

static void system_camera_move(system_payload_t payload,
                               allocator_t temporary_allocator) {
  entity_iter_t it = system_payload_entity_iter(&payload);
  input_t *input = resources_get_ty(input_t, payload.resources);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    vec2i inp = input_vector(input);
    vec2 wish =
        vec2_scale(vec2i_to_vec2(inp), CAMERA_SPEED * payload.delta_time);
    ptr->chunk_local_position = vec2_add(ptr->chunk_local_position, wish);
  }
}

static system_requirements_declaration_t system_decl_camera_move = {
    .name = "system_camera_move",
    .phase = SYSTEM_PHASE_UPDATE,
    .pin_to_main = false,
    .dependencies = 0,
    .dependency_count = 0,
    .entities_mut = ENTITY_KIND_CAMERA,
    .entities_const = 0,
    .runner = system_camera_move};

static void system_camera_set_projection(system_payload_t payload,
                                         allocator_t temporary_allocator) {
  shader_t *shader_ptr = resources_get_ty(shader_t, payload.resources);
  rendering_ctx_t *rendering_ctx =
      resources_get_ty(rendering_ctx_t, payload.resources);

  GLuint program = shader_ptr->gl_program;

  entity_iter_t it = system_payload_entity_iter(&payload);

  while (entity_iter_next(&it)) {
    entity_t *ptr = it.entity;
    if (ptr->kind != ENTITY_KIND_CAMERA)
      continue;

    mat4 projection = entity_camera_calculate_projection(rendering_ctx, ptr);
    rendering_ctx_set_projection(rendering_ctx, program, projection);
    break;
  }
}

static system_requirements_declaration_t system_decl_camera_set_projection = {
    .name = "system_camera_set_projection",
    .phase = SYSTEM_PHASE_UPDATE,
    .pin_to_main = false,
    .dependencies = 0,
    .dependency_count = 0,
    .entities_mut = ENTITY_KIND_CAMERA,
    .entities_const = 0,
    .runner = system_camera_set_projection};

#endif
