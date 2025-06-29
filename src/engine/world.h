#ifndef __SPACED_H__ENGINE_WORLD__
#define __SPACED_H__ENGINE_WORLD__

#include "../defs.h"
#include "../list.h"
#include "../map.h"
#include "../vec2i.h"

#include "entity.h"
#include <string.h>

typedef struct world_t {
  allocator_t allocator;
  list_t _entities;
  list_t vacant_entity_ids;
} world_t;

static void world_init(world_t *world) {
  world->allocator = allocator_new_malloc();

  list_init_ty(entity_t, &world->_entities, world->allocator);
  list_init_ty(entity_id_t, &world->vacant_entity_ids, world->allocator);
}

static void world_cleanup(world_t *world) { list_cleanup(&world->_entities); }

static entity_id_t world_reserve_entity_id(world_t *world) {
  entity_id_t id;
  if (world->vacant_entity_ids.size > 0) {
    list_pop_tail(&world->vacant_entity_ids, &id);
    return id;
  }

  id = world->_entities.size;

  entity_t e;
  memset(&e, 0, sizeof(entity_t));
  e.kind = ENTITY_KIND_TOMBSTONE;
  e.self_id = id;
  list_push_var(&world->_entities, e);

  return id;
}

static entity_id_t world_spawn_entity(world_t *world) {
  entity_id_t new_id = world_reserve_entity_id(world);
  u64 identity = entity_id_get_identity(new_id);

  entity_t *ptr = list_get_ty_ptr(entity_t, &world->_entities, identity);

  memset(ptr, 0, sizeof(entity_t));
  ptr->kind = ENTITY_KIND_TOMBSTONE;
  ptr->self_id = new_id;
  return new_id;
}

static entity_t *world_get_entity(world_t *world, entity_id_t id) {
  if (!world->_entities.size)
    return 0;

  entity_id_t identity = entity_id_get_identity(id);

  if (identity > world->_entities.size)
    return 0;

  entity_t *ptr = list_get_ty_ptr(entity_t, &world->_entities, identity);
  if (ptr->self_id != id) {
    if (ptr->kind == ENTITY_KIND_TOMBSTONE)
      return 0;

    PANIC(
        "Ruined invariant! Entity at %016lX index %013lX is indexed by %016lX.",
        id, entity_id_get_identity(id), entity_id_get_identity(ptr->self_id));
    return 0;
  }

  return ptr;
}

static sz world_count_entities(world_t *world) {
  if (world->vacant_entity_ids.size > world->_entities.size)
    return 0;
  return world->_entities.size - world->vacant_entity_ids.size;
}

static bool world_destroy_entity(world_t *world, entity_id_t id) {
  if (world->vacant_entity_ids.size > world->_entities.size)
    return false;

  u64 identity = entity_id_get_identity(id);
  u32 gen = entity_id_get_generation(id);
  entity_t *e = list_get_ty_ptr(entity_t, &world->_entities, identity);
  if (e->self_id != id)
    // The element must have already been deleted
    return false;

  // Clear the entity and increment the generation now.
  // This allows for any future indexing attempts against this
  // specific entity to fail due to generation mismatch
  memset(e, 0, sizeof(entity_t));
  e->kind = ENTITY_KIND_TOMBSTONE;
  gen = entity_id_get_generation(e->self_id);
  identity = entity_id_get_identity(e->self_id);
  e->self_id = entity_id_new(gen + 1, identity);

  list_push_var(&world->vacant_entity_ids, e->self_id);

  return true;
}

typedef struct {
  world_t *world;
  sz idx;
  entity_t *entity;
  entity_kind_mask_t mask;
} entity_iter_t;

static entity_iter_t world_entity_iter(world_t *world) {
  entity_iter_t ret;
  ret.world = world;
  ret.entity = 0;
  ret.idx = 0;
  ret.mask = (entity_kind_mask_t) ~(int)ENTITY_KIND_TOMBSTONE;
  return ret;
}

static entity_iter_t world_entity_iter_masked(world_t *world,
                                              entity_kind_mask_t mask) {
  entity_iter_t ret;
  ret.world = world;
  ret.entity = 0;
  ret.idx = 0;
  ret.mask = mask;
  return ret;
}

static bool entity_iter_next(entity_iter_t *it) {
  while (it->idx < it->world->_entities.size) {
    entity_t *candidate =
        list_get_ty_ptr(entity_t, &it->world->_entities, it->idx);
    it->idx++;
    if (candidate->kind & it->mask) {
      it->entity = candidate;
      return true;
    }
  }

  it->entity = 0;
  return false;
}

static entity_t *world_spawn_entity_construct(world_t *world,
                                              vec2i chunk_relative,
                                              vec2 chunk_local) {
  entity_id_t e = world_spawn_entity(world);
  entity_t *entity = world_get_entity(world, e);
  entity_init_construct(entity, world->allocator, chunk_relative, chunk_local);
  return entity;
}

static entity_t *world_spawn_entity_camera(world_t *world, vec2i chunk_relative,
                                           vec2 chunk_local, float scale) {
  entity_id_t e = world_spawn_entity(world);
  entity_t *entity = world_get_entity(world, e);

  entity_init_camera(entity, world->allocator, chunk_relative, chunk_local,
                     scale);

  return entity;
}

#endif
