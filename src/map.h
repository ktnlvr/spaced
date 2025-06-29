#ifndef __H__MAP__
#define __H__MAP__

#include <string.h>

#include "defs.h"
#include "memory.h"

/// @brief The load factor (in %) to be used when deciding
/// if its time to resize the map.
/// @memberof map_t
#define MAP_LOAD_FACTOR 75
/// @brief The maximum capactiy of the map. Will not resize
/// further independent of the @ref MAP_LOAD_FACTOR
/// @memberof map_t
#define MAP_MAX_CAPACITY (1 << 30)

typedef u32 map_key_t;

typedef map_key_t (*hash_function_f)(void *, sz);

typedef struct map_bucket_t {
  bool has_data;
  map_key_t key;
  struct map_bucket_t *next;
} map_bucket_t;

typedef struct map_t {
  /// @brief The allocator used for all 
  /// memory management, used for allocating
  /// both the main and the secondary storages
  allocator_t allocator;

  /// @brief The size of a single entry in bytes.
  /// Serves as a guardrail for runtime checking.
  sz entry_size;
  /// @brief The amount of elements currently in the map.
  sz size;
  /// @brief The amount of buckets available.
  sz capacity;

  /// @brief The raw byte storage used for the buckets.
  void *buckets;
} map_t;

/// @brief Retrieve the bucket of the `map` at index `idx`.
/// @private @memberof map_t
static map_bucket_t *map__get_bucket_at(map_t *map, sz idx) {
  sz block_size = sizeof(map_bucket_t) + map->entry_size;
  return (map_bucket_t *)(((char *)map->buckets) + idx * block_size);
}

/// @brief Initialize the map with a specific capacity and entry size. O(1)
/// @memberof map_t
static void map_init_with_capacity(map_t *map, allocator_t allocator,
                                   sz entry_size, sz capacity) {
  map->allocator = allocator;
  map->entry_size = entry_size;
  map->size = 0;
  map->capacity = capacity;

  sz block_size = sizeof(map_bucket_t) + entry_size;
  sz size = map->capacity * block_size;
  map->buckets = allocator_alloc(allocator, size);

  for (sz i = 0; i < map->capacity; i++) {
    map_bucket_t *b = map__get_bucket_at(map, i);
    b->next = 0;
    b->has_data = false;
  }
}

/// @brief Initializes the map with a fixed size and some initial capacity. O(1)
/// @memberof map_t
/// @param entry_size The size of the type stored inside the map
static void map_init(map_t *map, allocator_t allocator, sz entry_size) {
  map_init_with_capacity(map, allocator, entry_size, MAP_LOAD_FACTOR / 25 + 1);
}

/// @brief Initialize a typed map. O(1)
/// @memberof map_t
/// @param ty The type to be used inside the map
#define map_init_ty(ty, map, allocator) map_init(map, allocator, sizeof(ty))

/// @brief Cleanup the memory used by the map itself.
/// Importantly, does not clean the internal types stored
/// inside the map. O(N)
/// @memberof map_t
static void map_cleanup(map_t *map) {
  for (sz i = 0; i < map->capacity; i++) {
    map_bucket_t *b = map__get_bucket_at(map, i)->next;
    while (b) {
      map_bucket_t *next = b->next;
      allocator_free(map->allocator, b);
      b = next;
    }
  }

  allocator_free(map->allocator, map->buckets);
}

/// @brief Calculates if the map reached its @ref MAP_LOAD_FACTOR. O(1)
static bool map_should_grow(const map_t *map) {
  return map->capacity != MAP_MAX_CAPACITY &&
         (100 * map->size) / map->capacity >= MAP_LOAD_FACTOR;
}

/// @brief Calculates the next capacity for the map.
/// May return the same capacity if the @ref MAP_LOAD_FACTOR
/// is not reached. O(1)
static sz map_calculate_next_capacity(const map_t *map) {
  sz capacity = map->capacity;
  while (capacity * MAP_LOAD_FACTOR / 100 <= map->size) {
    capacity <<= 1;
    if (capacity >= MAP_MAX_CAPACITY)
      return MAP_MAX_CAPACITY;
  }

  return capacity;
}

static void map_insert(map_t *map, map_key_t key, void *data, sz size);

/// @brief Tries to grow the map. Doubles the size and reallocates all buckets.
/// Will not grow if the @ref MAP_LOAD_FACTOR is not reached. O(N)
/// @memberof map_t
static void map_grow(map_t *map) {
  if (!map_should_grow(map))
    return;

  sz new_capacity = map_calculate_next_capacity(map);

  map_t new_map;
  map_init_with_capacity(&new_map, map->allocator, map->entry_size, new_capacity);

  for (sz i = 0; i < map->capacity; i++) {
    map_bucket_t *bucket = map__get_bucket_at(map, i);
    if (!bucket->has_data)
      return;

    for (map_bucket_t *current = bucket; current; current = current->next)
      map_insert(&new_map, current->key, current + 1, map->entry_size);
  }

  map_cleanup(map);
  *map = new_map;
}

/// @brief Insert an element of the size `size` into the map.
/// Does an additional check to verify that the size of the insertion
/// matches the size set at initialization. O(1*)
/// @memberof map_t
static void map_insert(map_t *map, map_key_t key, void *data, sz size) {
  ASSERT(size == map->entry_size,
         "The size of an inserted entry (%d) must match the entry size defined "
         "at compile time (%d)",
         (int)size, (int)map->entry_size);

  if (map_should_grow(map))
    map_grow(map);

  sz idx = key % map->capacity;
  map_bucket_t *head = map__get_bucket_at(map, idx);

  if (!head->has_data) {
    head->has_data = true;
    head->key = key;
    head->next = 0;
    memcpy(head + 1, data, size);
    map->size++;
    return;
  }

  map_bucket_t *current = head;
  while (true) {
    if (current->has_data && current->key == key) {
      memcpy(current + 1, data, size);
      return;
    }

    if (!current->next)
      break;
    current = current->next;
  }

  map_bucket_t *new_node = (map_bucket_t *)allocator_alloc(
      map->allocator, sizeof(map_bucket_t) + map->entry_size);
  new_node->has_data = true;
  new_node->key = key;
  new_node->next = 0;
  memcpy(new_node + 1, data, size);
  current->next = new_node;
  map->size++;
}

#define map_insert_ty(ty, map, key, data) map_insert(map, key, data, sizeof(ty))

/// @brief Retrieves the element of the map at key `map_key`.
/// Returns @ref NULL if no element found. O(1*)
/// @memberof map_t
static void *map_get(map_t *map, map_key_t key) {
  sz idx = key % map->capacity;

  map_bucket_t *b = map__get_bucket_at(map, idx);
  if (!b->has_data)
    return 0;

  for (; b; b = b->next) {
    if (b->key == key)
      return b + 1;
  }

  return 0;
}

/// @brief Returns a typed pointer to a member at key `key`.
/// Returns `NULL` if the key is not present.
#define map_get_ty(ty, map, key) (ty *)map_get(map, key)

#endif
