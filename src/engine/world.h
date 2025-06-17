#ifndef __SPACED_H__ENGINE_WORLD__
#define __SPACED_H__ENGINE_WORLD__

#include "../defs.h"
#include "../list.h"

typedef struct world_chunk_t {
  i32 x, y;
  u32 tickets;
} world_chunk_t;

typedef struct world_t {
  list_t chunks;
} world_t;

static void world_init(world_t *world) {
  allocator_t malloc = allocator_new_malloc();
  list_init_ty(world_chunk_t, &world->chunks, malloc);
}

static void world_cleanup(world_t* world) {
  list_cleanup(&world->chunks);
}

#endif
