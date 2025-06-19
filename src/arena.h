#ifndef __SPACED_H__ARENA__
#define __SPACED_H__ARENA__

#include <string.h>

#include "defs.h"
#include "memory.h"

#define ARENA_DEFAULT_SIZE 0x10000

typedef struct {
  allocator_t allocator;
  byte *root;
  sz size;
  sz offset;
} arena_t;

static arena_t ARENA_GLOBAL;

static void arena_init(arena_t *arena, allocator_t alloc, sz size) {
  arena->allocator = alloc;
  arena->root = allocator_alloc_ty(byte, alloc, size);
  memset(arena->root, 0xCC, size);
  poison_memory_region(arena->root, size);

  arena->size = size;
  arena->offset = 0;
}

static void arena_init_default(arena_t *arena) {
  arena_init(arena, allocator_new_malloc(), ARENA_DEFAULT_SIZE);
}

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

static void arena_clear(arena_t *arena) {
  poison_memory_region(arena, arena->size);
  arena->offset = 0;
}

static void arena_cleanup(arena_t *arena) {
  ASSERT_(arena->root != 0, "Attempt to free an uninitialized arena");

  allocator_free(arena->allocator, arena->root);
  arena->root = 0;
  arena->offset = 0;
  arena->size = 0;
}

static void *allocator_arena__alloc(void *arena, sz size) {
  return arena_alloc((arena_t *)arena, size);
}

static void *allocator_arena__realloc(void *self, void *ptr, sz size) {
  arena_t *arena = (arena_t *)self;
  void *new_ptr = arena_alloc(arena, size);
  sz copy_size = size;

  memcpy(new_ptr, ptr, copy_size);
  return new_ptr;
}

static void allocator_arena__free(void *self, void *ptr) { return; }

static allocator_t arena_as_allocator(arena_t *arena) {
  allocator_t ret;
  ret.allocator = (void *)arena;
  ret.alloc = allocator_arena__alloc;
  ret.realloc = allocator_arena__realloc;
  ret.free = allocator_arena__free;

  return ret;
}

#define arena_alloc_ty(ty, arena, sz) (ty *)arena_alloc(arena, sz * sizeof(ty))

#endif
