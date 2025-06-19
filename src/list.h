#ifndef __SPACED_H__LIST__
#define __SPACED_H__LIST__

#include <memory.h>
#include <sanitizer/asan_interface.h>
#include <string.h>

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
  poison_memory_region(ls->data, ls->size * entry_size);
}

#define list_init_ty(ty, ls, alloc) list_init(ls, alloc, sizeof(ty))

static void list_init_copy(list_t *ls, allocator_t alloc, void *data,
                           sz entry_size, sz count) {
  ls->_alloc = alloc;
  ls->_entry = entry_size;
  ls->_capacity = entry_size * count;
  ls->data = allocator_alloc_copy(alloc, data, count * entry_size);
  ls->size = count;
  poison_memory_region(ls->data, ls->size * entry_size);
}

#define list_init_copy_ty(ty, ls, alloc, data, count)                          \
  list_init_copy(ls, alloc, data, sizeof(ty), count)

static void list_init_int(list_t *ls, allocator_t alloc) {
  list_init(ls, alloc, sizeof(i32));
}

// Grow the internal container and leave
// room for at least one extra item
static void list_grow(list_t *ls) {
  // Duplicate of Python's logic
  sz new_capacity = (ls->_capacity >> 3) + ls->_capacity + 6;

  ls->data = allocator_realloc(ls->_alloc, ls->data, new_capacity);
  ls->_capacity = new_capacity;
}

static void list_push(list_t *ls, const void *data) {
  if (ls->size == ls->_capacity)
    list_grow(ls);
  char *ptr = (char *)ls->data + ls->size * ls->_entry;

  unpoison_memory_region(ptr, ls->_entry);

  memcpy(ptr, data, ls->_entry);
  ls->size++;
}

static void list_insert(list_t *ls, sz idx, const void *data) {
  if (ls->size == ls->_capacity)
    list_grow(ls);

  if (idx == ls->size) {
    list_push(ls, data);
    return;
  }

  ASSERT(
      idx < ls->size,
      "Insertion index must be less must be within the list (idx=%d size=%d)",
      (int)idx, (int)ls->size);

  unpoison_memory_region((char *)ls->data + ls->size * ls->_entry,
                                ls->_entry);

  sz count_to_move = ls->size - idx;
  memmove(&((char *)ls->data)[(idx + 1) * ls->_entry],
          &((char *)ls->data)[idx * ls->_entry], count_to_move * ls->_entry);
  memcpy(&((char *)ls->data)[idx * ls->_entry], data, ls->_entry);
  ls->size++;
}

static void list_push_int(list_t *ls, const i32 value) {
  ASSERT(sizeof(value) == ls->_entry,
         "Size of a pushed value (%d) must match the entry size defined at "
         "initilization (%d)",
         value, (int)ls->_entry);

  list_push(ls, &value);
}

static void *list_get(list_t *ls, sz idx) {
  ASSERT(idx < ls->size, "Index %d too large (size=%d)", (int)idx,
         (int)ls->size);

  return (void *)(&((char *)ls->data)[idx * ls->_entry]);
}

#define list_get_ty(ty, ls, idx) *(ty *)list_get(ls, idx)

static void list_cleanup(list_t *ls) { allocator_free(ls->_alloc, ls->data); }

#endif
