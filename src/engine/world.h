#ifndef __SPACED_H__ENGINE_WORLD__
#define __SPACED_H__ENGINE_WORLD__

#include "../defs.h"
#include "../list.h"
#include "../map.h"
#include "../vec2i.h"

#include "construct.h"

typedef struct world_chunk_t {
  vec2i position;
  u32 tickets;
} world_chunk_t;

typedef struct world_t {
  map_t chunk_map;

  list_t constructs;
} world_t;

static void world_init(world_t *world) {
  allocator_t malloc = allocator_new_malloc();
  map_init_ty(world_chunk_t, &world->chunk_map, malloc);
}

static void world_cleanup(world_t *world) { map_cleanup(&world->chunk_map); }

#endif
