#ifndef __SPACED_H__ENGINE_WORLD__
#define __SPACED_H__ENGINE_WORLD__

#include "../defs.h"
#include "../list.h"
#include "../map.h"
#include "../vec2i.h"

#include "construct.h"
#include "entity.h"

typedef struct world_t {
  allocator_t allocator;
  list_t entities;
  list_t vacant_entity_ids;
} world_t;

static void world_init(world_t *world) {
  world->allocator = allocator_new_malloc();

  list_init_ty(entity_t, &world->entities, world->allocator);
  list_init_ty(entity_id_t, &world->vacant_entity_ids, world->allocator);
}

static void world_cleanup(world_t *world) { list_cleanup(&world->entities); }

static entity_id_t world_reserve_entity_id(world_t *world) {
  entity_id_t id;
  if (world->vacant_entity_ids.size > 0) {
    list_pop_tail(&world->vacant_entity_ids, &id);
    return id;
  }

  id = world->entities.size;
  list_push(&world->entities, &id);
  return id;
}

static entity_id_t world_spawn_entity(world_t *world) {
  entity_id_t new_id = world_reserve_entity_id(world);
  entity_t *ptr = list_get_ty_ptr(entity_t, &world->entities, new_id);
  memset(ptr, 0, sizeof(entity_t));
  ptr->self_id = new_id;
  return new_id;
}

static entity_t *world_get_entity(world_t *world, entity_id_t id) {
  entity_id_t identity = entity_id_get_identity(id);
  if (identity > world->entities.size)
    return 0;

  entity_t *ptr = list_get_ty_ptr(entity_t, &world->entities, identity);
  if (ptr->self_id != id) {
    PANIC("Attempt to locate a destroyed entity %08lX (expected gen=%03X, "
          "found gen=%03X)",
          id, entity_id_get_generation(id),
          entity_id_get_generation(ptr->self_id));
    return 0;
  }

  return ptr;
}

#endif
