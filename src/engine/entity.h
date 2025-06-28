#ifndef __SPACED_H__ENGINE_ENTITY__
#define __SPACED_H__ENGINE_ENTITY__

#include "../defs.h"
#include "../rendering/instances.h"
#include "../vec2i.h"
#include "construct.h"

typedef u64 entity_id_t;

#define ENTITY_ID_IDENTITY_MASK ((entity_id_t)0x000FFFFFFFFFFFFF)

static u32 entity_id_get_generation(entity_id_t id) {
  u64 mask = ~ENTITY_ID_IDENTITY_MASK;
  return (id & mask) >> __builtin_ctzl(mask);
}

static u64 entity_id_get_identity(entity_id_t id) {
  return id & ENTITY_ID_IDENTITY_MASK;
}

static u64 entity_id_new(u32 gen, u64 id) {
  u32 generation = gen % (~ENTITY_ID_IDENTITY_MASK >>
                          __builtin_ctzl(~ENTITY_ID_IDENTITY_MASK));
  u64 identity = id & ENTITY_ID_IDENTITY_MASK;

  return ((u64)generation << __builtin_ctzl(~ENTITY_ID_IDENTITY_MASK)) |
         identity;
}

typedef enum {
  ENTITY_KIND_TOMBSTONE = 0,
  ENTITY_KIND_CONSTRUCT,
  ENTITY_KIND_CAMERA,
  ENTITY_KIND_count,
} entity_kind_t;

typedef struct entity_t {
  entity_kind_t kind;
  entity_id_t self_id;

  // The position relative to the parent chunk
  // 0,0 for child entitites
  vec2i chunk_relative_position;
  vec2 chunk_local_position;

  union {
    struct {
      list_t components;
      instance_buffer_t instance;
    } construct;
    struct {
      entity_id_t target;
      float scale;
    } camera;
  };
} entity_t;

static void entity_init_construct(entity_t *entity, allocator_t alloc,
                                  vec2i chunk_relative, vec2 chunk_local) {
  entity->kind = ENTITY_KIND_CAMERA;
  entity->self_id = 0xCC;
  entity->chunk_relative_position = chunk_relative;
  entity->chunk_local_position = chunk_local;

  list_init_ty(component_t, &entity->construct.components, alloc);
  instance_buffer_init(&entity->construct.instance, alloc,
                       CONSTRUCT_MAXIMUM_COMPONENTS);
}

#endif
