#ifndef __SPACED_H__MEMORY__
#define __SPACED_H__MEMORY__

#include <string.h>

#include "defs.h"

typedef void *allocator_ptr;

typedef void *(mem_alloc_f)(allocator_ptr allocator, sz size);
typedef void *(mem_realloc_f)(allocator_ptr allocator, void *memory,
                              sz new_size);
typedef void(mem_free_f)(allocator_ptr allocator, void *memory);

typedef struct allocator_t {
  allocator_ptr allocator;
  mem_alloc_f *alloc;
  mem_realloc_f *realloc;
  mem_free_f *free;
} allocator_t;

static void *allocator_alloc(allocator_t allocator, sz size) {
  return allocator.alloc(allocator.allocator, size);
}

#define allocator_alloc_ty(ty, allocator, sz)                                  \
  (ty *)allocator_alloc(allocator, sizeof(ty) * sz)

static void *allocator_alloc_copy(allocator_t allocator, void *data, sz size) {
  void *ptr = allocator.alloc(allocator.allocator, size);
  memcpy(ptr, data, size);
  return ptr;
}

#define allocator_alloc_copy_ty(ty, allocator, data, sz)                       \
  (ty *)allocator_alloc_copy(allocator, data, sizeof(ty) * sz)

static void *allocator_realloc(allocator_t allocator, void *memory, sz size) {
  return allocator.realloc(allocator.allocator, memory, size);
}

static void allocator_free(allocator_t allocator, void *memory) {
  allocator.free(allocator.allocator, memory);
}

static void *allocator_malloc__alloc(allocator_ptr allocator, sz new_size) {
  return malloc(new_size);
}

static void *alloc_malloc__realloc(allocator_ptr allocator, void *memory,
                                   sz new_size) {
  return realloc(memory, new_size);
}

static void alloc_malloc__free(allocator_ptr allocator, void *memory) {
  free(memory);
}

static allocator_t allocator_new_malloc() {
  allocator_t ret;
  ret.allocator = 0;
  ret.alloc = allocator_malloc__alloc;
  ret.realloc = alloc_malloc__realloc;
  ret.free = alloc_malloc__free;

  return ret;
}

#endif
