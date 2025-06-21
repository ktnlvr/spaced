#ifndef __SPACED_H__NAMES__
#define __SPACED_H__NAMES__

#include "arena.h"
#include "defs.h"
#include "list.h"
#include "memory.h"

#ifndef NAME_ARENA_SIZE
#define NAME_ARENA_SIZE 0x10000
#endif

typedef u64 name_t;

static arena_t _name_arena = {};
static list_t _name_list;
static bool _name_list_is_locked = false;

static void names_init() {
  arena_init(&_name_arena, allocator_new_malloc(), NAME_ARENA_SIZE);
  allocator_t alloc = arena_as_allocator(&_name_arena);
  list_init_ty(const char *, &_name_list, alloc);
}

// Lock names at some point since all the fast strings
// should have been initialized and created
static void names_lock() { _name_list_is_locked = true; }

static name_t as_name(const char *str) {
  for (name_t i = 0; i < _name_list.size; i++)
    if (strcmp(str, list_get_ty(const char *, &_name_list, i)) == 0)
      return i;

  ASSERT(!_name_list_is_locked,
         "Name list must be unlocked to add new name `%s`", str);

  name_t idx = _name_list.size;

  allocator_t alloc = arena_as_allocator(&_name_arena);
  char *str_copy = allocator_alloc_copy_ty(char, alloc, str, strlen(str) + 1);
  list_push(&_name_list, &str_copy);

  return idx;
}

static void names_cleanup() {
  list_cleanup(&_name_list);
  arena_cleanup(&_name_arena);
}

#endif
