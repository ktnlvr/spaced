#ifndef __SPACED_H__ARENA__
#define __SPACED_H__ARENA__

#include <sanitizer/asan_interface.h>

#include "defs.h"

#define ARENA_DEFAULT_SIZE 0x10000

typedef struct {
  byte *root;
  sz size;
  sz offset;
} arena_t;

static arena_t ARENA_GLOBAL;

static void arena_init(arena_t *arena, sz size) {
  arena->root = (byte *)malloc(size);
  __asan_poison_memory_region(arena->root, size);

  arena->size = size;
  arena->offset = 0;
}

static void arena_init_default(arena_t *arena) {
  arena_init(arena, ARENA_DEFAULT_SIZE);
}

static void *arena_alloc(arena_t *arena, sz size) {
  // TODO: replace with asserts
  if (!arena->root)
    PANIC_("Arena not initialized");

  if (arena->offset + size > arena->size)
    PANIC_("Arena out of memory");

  void *ptr = arena->root + arena->offset;
  arena->offset += size;

  __asan_unpoison_memory_region(ptr, size);

  // Align the pointer correctly.
  // TODO: test if it actually speeds anything up
  arena->offset = (arena->offset + sizeof(void *) - 1) & ~(sizeof(void *) - 1);

  return ptr;
}

static void arena_clear(arena_t *arena) {
  __asan_poison_memory_region(arena, arena->size);
  arena->offset = 0;
}

static void arena_free(arena_t *arena) {
  if (arena->root == 0)
    PANIC_("Attempt to free an uninitialized arena");

  arena->root = 0;
  arena->offset = 0;
  arena->size = 0;
}

#define arena_alloc_ty(ty, arena, sz) (ty *)arena_alloc(arena, sz * sizeof(ty))

#endif
