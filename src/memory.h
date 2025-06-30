#ifndef __H__MEMORY__
#define __H__MEMORY__

#include <alloca.h>
#if defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#endif

#include <string.h>

#include "defs.h"

/// @brief Opaque type-erased pointer to an allocator. Carries no value other
/// than marking an argument to the function as being a type-erased allocator.
typedef void *allocator_ptr;

/// @brief Allocate `size` bytes using the `allocator`
typedef void *(mem_alloc_f)(allocator_ptr allocator, sz size);
/// @brief Reallocate the memory at `memory` to a new location with
// at least `size` bytes using the `allocator`.
typedef void *(mem_realloc_f)(allocator_ptr allocator, void *memory,
                              sz new_size);
/// @brief Give up the ownership of the `memory` within
/// the `allocator`.
typedef void(mem_free_f)(allocator_ptr allocator, void *memory);

/// @brief A generic allocator type passed by value. Contains a fallback
/// allocator and a set of function pointers for managing memory. Erases
/// the type of the internal allocator.
typedef struct allocator_t {
  /// @brief A pointer to the actual underyling allocator, type-erased.
  allocator_ptr allocator;
  mem_alloc_f *alloc;
  mem_realloc_f *realloc;
  mem_free_f *free;
} allocator_t;

/// @brief Proxy the allocation to the actual allocator.
/// @memberof allocator_t
static inline void *allocator_alloc(allocator_t allocator, sz size) {
  return allocator.alloc(allocator.allocator, size);
}

/// @brief Allocate a typed buffer of size `sz` using `allocator`.
/// @memberof allocator_t
#define allocator_alloc_ty(ty, allocator, sz)                                  \
  (ty *)allocator_alloc(allocator, sizeof(ty) * sz)

/// @brief Copy `size` bytes of data from `data` into a freshly allocated slice
/// using the `allocator`.
/// @memberof allocator_t
static inline void *allocator_alloc_copy(allocator_t allocator,
                                         const void *data, sz size) {
  void *ptr = allocator.alloc(allocator.allocator, size);
  memcpy(ptr, data, size);
  return ptr;
}

/// @brief Copy `sz` typedef elements of data from `data` into a freshly
/// allocated slice using the `allocator`.
/// @memberof allocator_t
#define allocator_alloc_copy_ty(ty, allocator, data, sz)                       \
  (ty *)allocator_alloc_copy(allocator, data, sizeof(ty) * sz)

/// @brief Move `memory` to a new memory region with at least `size` bytes.
/// If an allocator does not implement `realloc` it will be polyfilled.
/// @memberof allocator_t
static inline void *allocator_realloc(allocator_t allocator, void *memory,
                                      sz size) {
  ASSERT_(allocator.realloc, "This allocator does not implement `realloc`");
  return allocator.realloc(allocator.allocator, memory, size);
}

/// @brief Release `memory` back to the allocator.
/// @memberof allocator_t
static inline void allocator_free(allocator_t allocator, void *memory) {
  if (!allocator.free)
    return;
  allocator.free(allocator.allocator, memory);
}

/// @private @memberof allocator_ptr
static void *alloc_malloc__alloc(allocator_ptr allocator, sz new_size) {
  return malloc(new_size);
}

/// @private @memberof allocator_ptr
static void *alloc_malloc__realloc(allocator_ptr allocator, void *memory,
                                   sz new_size) {
  return realloc(memory, new_size);
}

/// @private @memberof allocator_ptr
static void alloc_malloc__free(allocator_ptr allocator, void *memory) {
  free(memory);
}

/// @brief Wrap `malloc` into an generic allocator interface
static allocator_t allocator_new_malloc() {
  allocator_t ret;
  ret.allocator = 0;
  ret.alloc = alloc_malloc__alloc;
  ret.realloc = alloc_malloc__realloc;
  ret.free = alloc_malloc__free;

  return ret;
}

static void *alloc_stack_alloc__alloc(allocator_ptr allocator, sz new_size) {
  return alloca(new_size);
}

static allocator_t allocator_new_stack_alloc() {
  allocator_t ret;
  ret.alloc = alloc_stack_alloc__alloc;
  ret.realloc = 0;
  ret.free = 0;
  return ret;
}

/// @brief If compiled with a memory sanitizer, any write to the
/// brief selected memory will crash the program. If applied multiple
/// times will still only be lifted once. Idempotent.
/// Pair of @ref unpoison_memory_region. O(1)
#if defined(__SANITIZE_ADDRESS__)
#define poison_memory_region(ptr, sz) __asan_poison_memory_region(ptr, sz)
#else
#define poison_memory_region(ptr, sz)
#endif

/// @brief Makes previously poisoned memory safe again. Idemponent.
/// Pair of @ref poison_memory_region. O(1)
#if defined(__SANITIZE_ADDRESS__)
#define unpoison_memory_region(ptr, sz) __asan_unpoison_memory_region(ptr, sz)
#else
#define unpoison_memory_region(ptr, sz)
#endif

#endif
