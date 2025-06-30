#ifndef __H__ARENA__
#define __H__ARENA__

#include <string.h>

#include "defs.h"
#include "memory.h"

#define ARENA_DEFAULT_SIZE 0x10000

/// @brief The Arena allocator is a single continous block of memory.
///
/// Any deletions from the block do nothing except mark the memory
/// as poisoned. The intention to use the arena for objects that
/// share a lifetime, for instance all objects that live for 1
/// frame.
typedef struct {
  /// @brief The allocator that the arena will forward all the
  /// allocation requests to.
  allocator_t fallback;
  /// @brief The starting address of the arena.
  byte *root;
  /// @brief The total amount of bytes the arena can handle.
  sz size;
  /// @brief The next location to put the data at.
  sz offset;
} arena_t;

/// @brief The single global arena. Declared here instead of
/// specific files to prevent aliasing issues.
static arena_t ARENA_GLOBAL;

/// @brief Initializes the memory arena.
/// @memberof arena_t
static void arena_init(arena_t *arena, allocator_t fallback, sz size) {
  arena->fallback = fallback;
  arena->root = allocator_alloc_ty(byte, fallback, size);
  memset(arena->root, 0xCC, size);
  poison_memory_region(arena->root, size);

  arena->size = size;
  arena->offset = 0;
}

/// @brief Initializes the memory arena using `malloc`
/// with @ref ARENA_DEFAULT_SIZE bytes of memory.
static void arena_init_default(arena_t *arena) {
  arena_init(arena, allocator_new_malloc(), ARENA_DEFAULT_SIZE);
}

/// @brief Allocate `size` bytes data inside the arena. O(1)
static void *arena_alloc(arena_t *arena, sz size) {
  ASSERT_(arena->root != 0, "Expected arena to be initialized");

  ASSERT(
      arena->offset + size < arena->size,
      "Expected enough space in the arena (capacity=%d, busy=%d, requested=%d)",
      (int)arena->size, (int)arena->offset, (int)size);

  void *ptr = arena->root + arena->offset;
  arena->offset += size;

  unpoison_memory_region(ptr, size);

  // Align the pointer correctly.
  // TODO: test if it actually speeds anything up
  arena->offset = (arena->offset + sizeof(void *) - 1) & ~(sizeof(void *) - 1);

  return ptr;
}

/// @brief Clears the memory arena entirely and marks all
/// the memory as poison. Importantly, does not release memory. O(1)
/// @memberof arena_t
static void arena_clear(arena_t *arena) {
  poison_memory_region(arena, arena->size);
  arena->offset = 0;
}

/// @brief Release all the allocated memory.
/// @memberof arena_t
static void arena_cleanup(arena_t *arena) {
  ASSERT_(arena->root != 0, "Attempt to free an uninitialized arena");

  allocator_free(arena->fallback, arena->root);
  arena->root = 0;
  arena->offset = 0;
  arena->size = 0;
}

/// @private @memberof arena_t
static void *allocator_arena__alloc(void *arena, sz size) {
  return arena_alloc((arena_t *)arena, size);
}

/// @private @memberof arena_t
static void *allocator_arena__realloc(void *self, void *ptr, sz size) {
  arena_t *arena = (arena_t *)self;
  void *new_ptr = arena_alloc(arena, size);
  sz copy_size = size;

  memcpy(new_ptr, ptr, copy_size);
  return new_ptr;
}

/// @private @memberof arena_t
static void allocator_arena__free(void *self, void *ptr) {
  // TODO: poison the memory region
  return;
}

/// @brief A generic function to type-erase the @ref arena_t
/// into an @ref allocator_t.
/// @memberof arena_t
static allocator_t arena_as_allocator(arena_t *arena) {
  allocator_t ret;
  ret.allocator = (void *)arena;
  ret.alloc = allocator_arena__alloc;
  ret.realloc = allocator_arena__realloc;
  ret.free = allocator_arena__free;

  return ret;
}

/// @brief Allocate a typed array inside the arena.
/// @memberof arena_t
#define arena_alloc_ty(ty, arena, sz) (ty *)arena_alloc(arena, sz * sizeof(ty))

#endif
