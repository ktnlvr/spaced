#ifndef __SPACED_H__ENGINE_ENTITY__
#define __SPACED_H__ENGINE_ENTITY__

#include "../defs.h"
#include "../rendering/instances.h"
#include "../vec2i.h"

typedef u64 entity_id_t;

#define ENTITY_ID_IDENTITY_MASK ((entity_id_t)0x000FFFFFFFFFFFFF)
#define CONSTRUCT_MAXIMUM_COMPONENTS 255

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
      bool is_dirty;
    } as_construct;
    struct {
      entity_id_t target;
      float scale;
    } as_camera;
  };
} entity_t;

typedef enum {
  COMPONENT_KIND_MESH,
  COMPONENT_KIND_THRUSTER,
  COMPONENT_KIND_ENGINE,
  COMPONENT_KIND_count,
} component_kind_t;

typedef struct {
  component_kind_t kind;
  vec2i offset;
} component_t;

static void entity_init_construct(entity_t *entity, allocator_t alloc,
                                  vec2i chunk_relative, vec2 chunk_local) {
  entity->kind = ENTITY_KIND_CONSTRUCT;
  entity->chunk_relative_position = chunk_relative;
  entity->chunk_local_position = chunk_local;

  list_init_ty(component_t, &entity->as_construct.components, alloc);
  instance_buffer_init(&entity->as_construct.instance, alloc,
                       CONSTRUCT_MAXIMUM_COMPONENTS);
  entity->as_construct.is_dirty = false;
}

static void entity_construct_add_component(entity_t *entity, vec2i at,
                                           component_t component) {
  component.offset = at;
  list_push_var(&entity->as_construct.components, component);
  entity->as_construct.is_dirty = true;
}

static component_t component_new_mesh() {
  component_t ret;
  ret.kind = COMPONENT_KIND_MESH;
  return ret;
}

#endif
