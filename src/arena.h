#ifndef __SPACED_H__ARENA__
#define __SPACED_H__ARENA__

#include <sanitizer/asan_interface.h>

#include "defs.h"

#define ARENA_DEFAULT_SIZE 0x10000

static byte *arena_root;
static sz arena_size = 0;
static sz arena_offset = 0;

static void arena_init(sz size) {
  arena_root = (byte *)malloc(size);
  __asan_poison_memory_region(arena_root, size);

  arena_size = size;
  arena_offset = 0;
}

static void arena_init_default() { arena_init(ARENA_DEFAULT_SIZE); }

static void *arena_alloc(sz size) {
  // TODO: replace with asserts
  if (!arena_root)
    PANIC_("Arena not initialized");

  if (arena_offset + size > arena_size)
    PANIC_("Arena out of memory");

  void *ptr = arena_root + arena_offset;
  arena_offset += size;

  __asan_unpoison_memory_region(ptr, size);

  // Align the pointer correctly.
  // TODO: test if it actually speeds anything up
  arena_offset = (arena_offset + sizeof(void *) - 1) & ~(sizeof(void *) - 1);

  return ptr;
}

static void arena_clear() {
  __asan_poison_memory_region(arena_root, arena_size);
  arena_offset = 0;
}

static void arena_free() {
  if (arena_root == 0)
    PANIC_("Attempt to free an uninitialized arena");

  arena_root = 0;
  arena_offset = 0;
  arena_size = 0;
}

#define arena_alloc_ty(ty, sz) (ty *)arena_alloc(sz * sizeof(ty))

#endif
