#ifndef __SPACED_H__LIST__
#define __SPACED_H__LIST__

#include <memory.h>
#include <string.h>
#include <sanitizer/asan_interface.h>

#include "defs.h"
#include "memory.h"

#define LIST_DEFAULT_CAPACITY 6

typedef struct {
  allocator_t _alloc;
  sz _entry;
  sz _capacity;
  sz size;

  void *data;
} list_t;

static void list_init(list_t *ls, allocator_t alloc, sz entry_size) {
  ls->_alloc = alloc;
  ls->_entry = entry_size;
  ls->_capacity = LIST_DEFAULT_CAPACITY;
  // TODO(Artur): ASAN
  ls->data = allocator_alloc(alloc, entry_size * ls->_capacity);
  ls->size = 0;
  __asan_poison_memory_region(ls->data, ls->size * entry_size);
}

#define list_init_ty(ty, ls, alloc) list_init(ls, alloc, sizeof(ty))

static void list_init_int(list_t *ls, allocator_t alloc) {
  list_init(ls, alloc, sizeof(i32));
}

// Grow the internal container and leave
// room for at least one extra item
static void list_grow(list_t *ls) {
  // Duplicate of Python's logic
  sz new_capacity = (ls->_capacity >> 3) + ls->_capacity + 6;

  allocator_realloc(ls->_alloc, ls->data, new_capacity);
  ls->_capacity = new_capacity;
}

static void list_push(list_t *ls, const void *data) {
  if (ls->size == ls->_capacity)
    list_grow(ls);
  char *ptr = (char *)ls->data + ls->size * ls->_entry;

  __asan_unpoison_memory_region(ptr, ls->_entry);

  memcpy(ptr, data, ls->_entry);
  ls->size++;
}

static void list_push_int(list_t *ls, const i32 value) {
  if (sizeof(value) != ls->_entry)
    PANIC("Pushing an int into a list with entry size %d, is that the right "
          "list?",
          (int)ls->_entry);

  list_push(ls, &value);
}

static void list_cleanup(list_t *ls) { allocator_free(ls->_alloc, ls->data); }

#endif
