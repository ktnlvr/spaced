#ifndef __SPACED_H__ENGINE_COMPONENT__
#define __SPACED_H__ENGINE_COMPONENT__

#include "../defs.h"
#include "../list.h"
#include "../map.h"
#include "../memory.h"
#include "../misc.h"

typedef u32 entity_t;

#define ENTITY_TOMBSTONE (entity_t)0

#define ENTITY_GENERATION_MASK ((u64)0xFFF0000000000000)

static u32 entity_generation(entity_t entt) {
  return (entt & ENTITY_GENERATION_MASK) >>
         __builtin_ctzll(ENTITY_GENERATION_MASK);
}

static u64 entity_identity(entity_t entt) {
  return entt & ~ENTITY_GENERATION_MASK;
}

typedef u32 component_id_t;

static u64 hash_component_ids(component_id_t *ids, sz count) {
  u64 ret;
  for (sz i = 0; i < count; i++)
    ret = hash_combine_com_u64(ret, ids[i]);
  return ret;
}

/// Stores all the components of a specific archetype
typedef struct archetype_store_t {
  /// Combined commutative hash of all the component IDs
  u64 fingerprint;
  list_t component_ids;

  /// Allocator used for all allocations
  allocator_t allocator;

  /// The actual components
  list_t data;
} archetype_store_t;

static void archetype_store_init(archetype_store_t *arch, allocator_t alloc,
                                 sz payload_size, component_id_t *ids,
                                 sz id_count) {
  arch->allocator = alloc;
  list_init_copy_ty(component_id_t, &arch->component_ids, alloc, ids, id_count);

  arch->fingerprint = hash_component_ids(ids, id_count);

  // TODO: respect alignment
  sz entry_size = sizeof(entity_t) + payload_size;
  list_init(&arch->data, arch->allocator, entry_size);
}

static void archetype_store_cleanup(archetype_store_t *arch) {
  list_cleanup(&arch->component_ids);
  list_cleanup(&arch->data);
}

// Describes active entities and their components
typedef struct store_t {
  allocator_t allocator;
  map_t archetype_stores;
} store_t;

static void store_init(store_t *store, allocator_t alloc) {
  store->allocator = alloc;
  map_init_ty(archetype_store_t, &store->archetype_stores, alloc);
}

static void store_cleanup(store_t *store) {
  map_cleanup(&store->archetype_stores);
}

#endif
