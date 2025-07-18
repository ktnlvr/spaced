#ifndef __H__SCHEDULE_RESOURCE__
#define __H__SCHEDULE_RESOURCE__

#include "../map.h"
#include "../memory.h"
#include "../names.h"

typedef void (*cleanup_f)(void *);

typedef struct {
  void *ptr;
  cleanup_f cleanup;
} resources__container_t;

typedef struct {
  allocator_t alloc;
  map_t storage;
} resources_t;

static void resources_init(resources_t *resources, allocator_t alloc) {
  resources->alloc = alloc;
  map_init_ty(resources__container_t, &resources->storage, alloc);
}

static void resource_add_with_cleanup(resources_t *resources, name_t name,
                                      void *resource, cleanup_f cleanup) {
  ASSERT__(!map_contains(&resources->storage, name));
  resources__container_t container;
  container.cleanup = cleanup;
  map_insert(&resources->storage, name, &container,
             sizeof(resources__container_t));
}

#define resources_add_ty(ty, resources, res)                                   \
  resource_add_with_cleanup(resources, as_name(#ty), &res, 0)

static void *resources_get(resources_t *resources, name_t name) {
  void *ptr = map_get(&resources->storage, name);
  ASSERT(ptr, "Resource with name `%s` (name=%d) was never added",
         name_as_str(name), name);
  return ptr;
}

#define resources_get_ty(ty, resources)                                        \
  ((ty *)resources_get(resources, as_name(#ty)))

#define resources_get_world(res) resources_get_ty(world_t, res)

#endif
