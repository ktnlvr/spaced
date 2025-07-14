#ifndef __H__SYSTEMS_REQUIRE__
#define __H__SYSTEMS_REQUIRE__

#include "../engine/entity.h"
#include "../engine/input.h"
#include "../engine/world.h"
#include "../names.h"
#include "../rendering/context.h"

typedef enum {
  SYSTEM_PHASE_PRE_UPDATE,
  SYSTEM_PHASE_UPDATE,
  SYSTEM_PHASE_FIXED_UPDATE,
  SYSTEM_PHASE_POST_UPDATE,
  SYSTEM_PHASE_PRE_RENDER,
  SYSTEM_PHASE_RENDER,
  SYSTEM_PHASE_POST_RENDER,
  SYSTEM_PHASE_count,
} system_phase_t;

static const char *system_phase_to_str(system_phase_t phase) {
  switch (phase) {
  case SYSTEM_PHASE_PRE_UPDATE:
    return "PRE_UPDATE";
  case SYSTEM_PHASE_UPDATE:
    return "UPDATE";
  case SYSTEM_PHASE_FIXED_UPDATE:
    return "FIXED_UPDATE";
  case SYSTEM_PHASE_POST_UPDATE:
    return "POST_UPDATE";
  case SYSTEM_PHASE_PRE_RENDER:
    return "PRE_RENDER";
  case SYSTEM_PHASE_RENDER:
    return "RENDER";
  case SYSTEM_PHASE_POST_RENDER:
    return "RENDER";
  default:
    return 0;
  }
}

/// @brief Sentinel value. Marks the system resourc as required.
/// The scheduler must guarantee exclusive access.
#define SYSTEM_REQ_PTR_REQUIRED ((void *)1)
/// @brief Sentinel value. Marks the system resource as unused.
/// The scheduler must provide a `NULL` pointer.
#define SYSTEM_REQ_PTR_UNUSED ((void *)0)

#define SYSTEM_REQ_NO_DEPS 0, 0

typedef struct {
  void *system_specific_data;
  world_t *world;
  input_t *input;
  rendering_ctx_t *rendering_ctx;

  entity_kind_mask_t _entity_kinds_mut;
  entity_kind_mask_t _entity_kinds_const;

  double delta_time;
  double fixed_delta_time;
} system_payload_t;

typedef void (*system_runner_f)(system_payload_t payload,
                                allocator_t temporary_allocator);

typedef enum {
  SYSTEM_RESOURCE_MASK_WORLD = 0b1,
  SYSTEM_RESOURCE_MASK_INPUT = 0b10,
  SYSTEM_RESOURCE_MASK_RENDERING_CONTEXT = 0b100,
  SYSTEM_RESOURCE_MASK_SYSTEM_SPECIFIC_DATA = 0b1000,
} system_resource_mask_t;

typedef struct {
  system_phase_t phase;
  name_t name;
  entity_kind_mask_t _entity_kinds_mut;
  entity_kind_mask_t _entity_kinds_const;

  name_t *depends_on;
  u32 depends_on_count;

  system_resource_mask_t resources;
  void *system_specific_data;

  bool pin_to_main;
} system_req_t;

typedef struct {
  const char *name;
  system_phase_t phase;

  entity_kind_mask_t entities_const;
  entity_kind_mask_t entities_mut;

  const char **dependencies;
  u32 dependency_count;

  system_resource_mask_t resources;
  bool pin_to_main;

  system_runner_f runner;
} system_requirements_declaration_t;

static system_payload_t system_payload_new(world_t *world, input_t *input,
                                           rendering_ctx_t *rendering_ctx,
                                           double dt, double fdt) {
  system_payload_t ret = {};

  ret.system_specific_data = 0;
  ret._entity_kinds_const = 0;
  ret._entity_kinds_mut = 0;

  ret.world = world;
  ret.input = input;
  ret.rendering_ctx = rendering_ctx;

  ret.delta_time = dt;
  ret.fixed_delta_time = fdt;

  return ret;
}

/// @brief Copy the parameters specified in the @ref requirements into a new
/// payload
/// @return the payload with the skipped fields zero-ed out
static system_payload_t system_payload_new_inherit(system_req_t requirements,
                                                   system_payload_t *parent) {
  bool needs_system_specific_ptr =
      requirements.resources & SYSTEM_RESOURCE_MASK_SYSTEM_SPECIFIC_DATA;
  ASSERT__(needs_system_specific_ptr ^ (parent->system_specific_data == 0));

  system_payload_t ret = {};

  if (parent == 0)
    return ret;

  if (requirements.resources & SYSTEM_RESOURCE_MASK_WORLD ||
      requirements._entity_kinds_const || requirements._entity_kinds_mut) {
    ASSERT__(parent->world);
    ret.world = parent->world;
  }

  if (requirements.resources & SYSTEM_RESOURCE_MASK_INPUT) {
    ASSERT__(parent->input);
    ret.input = parent->input;
  }

  if (requirements.resources & SYSTEM_RESOURCE_MASK_RENDERING_CONTEXT) {
    ASSERT__(parent->rendering_ctx);
    ret.rendering_ctx = parent->rendering_ctx;
  }

  if (requirements.resources & SYSTEM_RESOURCE_MASK_SYSTEM_SPECIFIC_DATA) {
    ASSERT__(requirements.system_specific_data);
    ret.system_specific_data = requirements.system_specific_data;
  }

  ASSERT__(__builtin_popcount(parent->_entity_kinds_mut) <=
           __builtin_popcount(parent->_entity_kinds_const));

  ret._entity_kinds_const =
      parent->_entity_kinds_const | parent->_entity_kinds_mut;
  ret._entity_kinds_mut = parent->_entity_kinds_mut;

  return ret;
}

static system_req_t system_req_new(name_t name,
                                   system_resource_mask_t resources) {
  system_req_t ret;

  ret._entity_kinds_const = (entity_kind_mask_t)0;
  ret._entity_kinds_mut = (entity_kind_mask_t)0;

  ret.depends_on = 0;
  ret.depends_on_count = 0;

  ret.pin_to_main = false;

  ret.name = name;
  ret.resources = resources;
  ret.system_specific_data = 0;

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
  ASSERT_(payload->world,
          "World must be requested to retrieve the entity iterator");

  entity_iter_t iter = world_entity_iter_masked(
      payload->world, (entity_kind_mask_t)((u32)payload->_entity_kinds_const |
                                           (u32)payload->_entity_kinds_mut));

  return iter;
}

#endif
