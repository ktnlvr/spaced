#ifndef __SPACED_H__ENGINE_WORLD__
#define __SPACED_H__ENGINE_WORLD__

#include "../defs.h"
#include "../list.h"
#include "../map.h"
#include "../vec2i.h"

#include "construct.h"

typedef enum {
  ENTITY_KIND_CONSTRUCT,
  ENTITY_KIND_RENDER_QUADS,
} entity_kind_t;

typedef struct entity_t {
  struct entity_t *next;
  u32 canary;
  entity_kind_t kind;
  vec2 chunk_position;
  // The AABB extents of the entity
  vec2 extents;
} entity_t;

typedef entity_t *entity_ptr_t;

typedef struct world_chunk_t {
  vec2i position;
  u32 tickets;
} world_chunk_t;

typedef struct world_t {
  allocator_t allocator;
  entity_t *entities;
  map_t chunk_map;
} world_t;

static void world_init(world_t *world) {
  allocator_t malloc = allocator_new_malloc();
  map_init_ty(world_chunk_t, &world->chunk_map, malloc);
  world->entities = 0;
  world->allocator = malloc;
}

static void world_cleanup(world_t *world) { map_cleanup(&world->chunk_map); }

static entity_ptr_t world_alloc_entity(world_t *world, entity_kind_t kind) {
  sz payload = 0;
  switch (kind) {
  case ENTITY_KIND_CONSTRUCT:
    payload = sizeof(construct_t);
    break;
  case ENTITY_KIND_RENDER_QUADS:
    payload = sizeof(render_quads_t);
    break;
  default:
    PANIC("Unkown entity kind %d", kind);
  }

  entity_ptr_t entt = (entity_ptr_t)allocator_alloc(world->allocator,
                                                    sizeof(entity_t) + payload);
  entt->canary = (u64)entt & 0xFFFFFFFF;
  entt->kind = kind;
  entt->chunk_position = vec2_new(0., 0.);
  entt->extents = vec2_new(0., 0.);
  entt->next = 0;
  return entt;
}

#define DEFINE_ENTITY_PTR_AS(tname, knd)                                       \
  static tname##_t *entity_ptr_as_##tname(entity_ptr_t entt) {                 \
    ASSERT__(entt->kind == knd);                                               \
    return (tname##_t *)(entt + 1);                                            \
  }

DEFINE_ENTITY_PTR_AS(construct, ENTITY_KIND_CONSTRUCT)
DEFINE_ENTITY_PTR_AS(render_quads, ENTITY_KIND_RENDER_QUADS)

static entity_ptr_t to_entity_ptr(void *ptr) {
  entity_ptr_t entt = ((entity_ptr_t)ptr - 1);
  ASSERT__(entt->canary == ((u64)entt & 0xFFFFFFFF));
  return entt;
}

static construct_t *world_spawn_construct(world_t *world,
                                          render_quads_t *render_quads_entt) {
  construct_t *construct =
      entity_ptr_as_construct(world_alloc_entity(world, ENTITY_KIND_CONSTRUCT));

  construct_init(construct);

  return construct;
}

static render_quads_t *world_spawn_render_quads(world_t *world, sz capacity) {
  render_quads_t *quads = entity_ptr_as_render_quads(
      world_alloc_entity(world, ENTITY_KIND_RENDER_QUADS));

  render_quads_init(quads, world->allocator, capacity);

  return quads;
}

#endif
