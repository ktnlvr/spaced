#ifndef __H__SYSTEMS_REQUIRE__
#define __H__SYSTEMS_REQUIRE__

#include "../engine/entity.h"
#include "../engine/input.h"
#include "../engine/world.h"
#include "../names.h"
#include "../rendering/context.h"

/// @brief Sentinel value. Marks the system resourc as required.
/// The scheduler must guarantee exclusive access.
#define SYSTEM_REQ_PTR_REQUIRED ((void *)1)
/// @brief Sentinel value. Marks the system resource as unused.
/// The scheduler must provide a `NULL` pointer.
#define SYSTEM_REQ_PTR_UNUSED ((void *)0)

#define SYSTEM_REQ_NO_DEPS 0

typedef struct {
  name_t name;
  entity_kind_mask_t _entity_kinds_mut;
  entity_kind_mask_t _entity_kinds_const;

  void *system_specific_data;
  world_t *world;
  input_t *input;
  rendering_ctx_t *rendering_ctx;

  double delta_time;
  double fixed_delta_time;

  bool pin_to_main;
} system_req_t;

static system_req_t system_req_new() {
  system_req_t ret;

  ret._entity_kinds_const = (entity_kind_mask_t)0;
  ret._entity_kinds_mut = (entity_kind_mask_t)0;

  ret.system_specific_data = 0;
  ret.world = 0;
  ret.input = 0;
  ret.rendering_ctx = 0;

  ret.pin_to_main = false;

  return ret;
}

static void system_req_fill_in(system_req_t *target,
                               const system_req_t parent) {
  target->_entity_kinds_const = parent._entity_kinds_const;
  target->_entity_kinds_mut = parent._entity_kinds_mut;
  if (parent._entity_kinds_const || parent._entity_kinds_mut || target->world) {
    ASSERT_(parent.world, "world_t expected in parent, but not filled in");
    target->world = parent.world;
  }

  if (target->rendering_ctx) {
    ASSERT_(parent.world,
            "rendering_ctx_t expected in parent, but not filled in");
    target->rendering_ctx = parent.rendering_ctx;
  }

  if (target->input) {
    ASSERT_(parent.input, "input_t expected in parent, but not filled in");
    target->input = parent.input;
  }

  ASSERT(isfinite(parent.delta_time), "Finite delta_time expected, got %lf",
         parent.delta_time);
  target->delta_time = parent.delta_time;

  ASSERT(isfinite(parent.fixed_delta_time),
         "Finite fixed_delta_time expected, got %lf", parent.fixed_delta_time);
  target->fixed_delta_time = parent.fixed_delta_time;
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

#endif
