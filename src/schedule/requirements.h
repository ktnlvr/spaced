#ifndef __H__SCHEDULE_REQUIREMENTS__
#define __H__SCHEDULE_REQUIREMENTS__

#include "../engine/entity.h"
#include "../engine/input.h"
#include "../engine/world.h"
#include "../names.h"
#include "../rendering/context.h"
#include "../schedule/resource.h"
#include "phase.h"

typedef struct {
  resources_t *resources;

  entity_kind_mask_t _entity_kinds_mut;
  entity_kind_mask_t _entity_kinds_const;

  double delta_time;
  double fixed_delta_time;
} system_payload_t;

typedef void (*system_runner_f)(system_payload_t payload,
                                allocator_t temporary_allocator);

typedef struct {
  system_phase_t phase;
  name_t name;
  entity_kind_mask_t _entity_kinds_mut;
  entity_kind_mask_t _entity_kinds_const;

  name_t *depends_on;
  u32 depends_on_count;

  bool pin_to_main;
} system_req_t;

typedef struct {
  const char *name;
  system_phase_t phase;

  entity_kind_mask_t entities_const;
  entity_kind_mask_t entities_mut;

  const char **dependencies;
  u32 dependency_count;
  bool pin_to_main;

  system_runner_f runner;
} system_requirements_declaration_t;

static system_payload_t system_payload_new(resources_t *res, double dt,
                                           double fdt) {
  system_payload_t ret = {};

  ret._entity_kinds_const = 0;
  ret._entity_kinds_mut = 0;

  ret.delta_time = dt;
  ret.fixed_delta_time = fdt;

  return ret;
}

/// @brief Copy the parameters specified in the @ref requirements into a new
/// payload
/// @return the payload with the skipped fields zero-ed out
static system_payload_t system_payload_new_inherit(system_req_t requirements,
                                                   system_payload_t *parent) {
  system_payload_t ret = {};

  ret.resources = parent->resources;
  ret._entity_kinds_const =
      parent->_entity_kinds_const | parent->_entity_kinds_mut;
  ret._entity_kinds_mut = parent->_entity_kinds_mut;

  return ret;
}

static system_req_t system_req_new(name_t name) {
  system_req_t ret = {};

  ret._entity_kinds_const = (entity_kind_mask_t)0;
  ret._entity_kinds_mut = (entity_kind_mask_t)0;

  ret.depends_on = 0;
  ret.depends_on_count = 0;

  ret.pin_to_main = false;

  ret.name = name;

  return ret;
}

static void system_req_entity_kinds_const(system_req_t *req,
                                          entity_kind_mask_t entity_kind_mask) {
  req->_entity_kinds_const =
      (entity_kind_mask_t)((int)req->_entity_kinds_mut | (int)entity_kind_mask);
}

static void system_req_entity_kinds_mut(system_req_t *req,
                                        entity_kind_mask_t entity_kind_mask) {
  req->_entity_kinds_mut =
      (entity_kind_mask_t)((int)req->_entity_kinds_mut | (int)entity_kind_mask);
  system_req_entity_kinds_const(req, entity_kind_mask);
}

static entity_iter_t system_payload_entity_iter(system_payload_t *payload) {
  world_t *world = resources_get_world(payload->resources);

  entity_iter_t iter = world_entity_iter_masked(
      world, (entity_kind_mask_t)((u32)payload->_entity_kinds_const |
                                  (u32)payload->_entity_kinds_mut));

  return iter;
}

#endif
