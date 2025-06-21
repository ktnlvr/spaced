#ifndef __SPACED_H__ENGINE_CONSTRUCT__
#define __SPACED_H__ENGINE_CONSTRUCT__

#include "../rendering/quads.h"
#include "../vec2i.h"

typedef enum {
  COMPONENT_KIND_MESH,
  COMPONENT_KIND_THRUSTER,
  COMPONENT_KIND_ENGINE,
} component_kind_t;

typedef struct {
  component_kind_t kind;
  vec2i offset;
} component_t;

static void component_init_mesh(component_t *comp, vec2i offset) {
  comp->kind = COMPONENT_KIND_MESH;
  comp->offset = offset;
}

typedef struct construct_t {
  allocator_t alloc;
  list_t components;

  render_quads_t *quads;
} construct_t;

static void construct_init(construct_t *cons, allocator_t alloc) {
  list_init_ty(component_t, &cons->components, alloc);
}

static void construct_push_component(construct_t *cons, component_t component) {
  list_push(&cons->components, &component);

  if (cons->quads) {
    instance_t instance;
    instance.position = vec2i_to_vec2(component.offset);

    instance_buffer_push(&cons->quads->instances, instance);
  }
}

#endif
