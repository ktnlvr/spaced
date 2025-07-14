#ifndef __H__SYSTEMS_PHYSICS__
#define __H__SYSTEMS_PHYSICS__

#include "require.h"

static void system_integrate_velocity(system_payload_t payload,
                                      allocator_t temporary_allocator) {
  entity_iter_t it = system_payload_entity_iter(&payload);
  while (entity_iter_next(&it)) {
    ASSERT__(it.entity->kind == ENTITY_KIND_CONSTRUCT);
    // TODO: overflow the position correctly
    vec2 *v = &it.entity->as_construct.velocity;
    *v = vec2_add(*v, vec2_scale(*v, payload.delta_time));
    printf("%f %f\n", v->x, v->y);
  }
}

static system_requirements_declaration_t system_decl_integrate_velocity = {
    .name = "system_integrate_velocity",
    .dependencies = 0,
    .dependency_count = 0,
    .entities_const = 0,
    .entities_mut = ENTITY_KIND_CONSTRUCT,
    .phase = SYSTEM_PHASE_FIXED_UPDATE,
    .resources = SYSTEM_RESOURCE_MASK_WORLD,
    .runner = system_integrate_velocity};

#endif
