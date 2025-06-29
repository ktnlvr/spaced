#ifndef __SPACED_H__ENGINE_ENTITY__
#define __SPACED_H__ENGINE_ENTITY__

#include "../defs.h"
#include "../mat4.h"
#include "../rendering/context.h"
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
  ENTITY_KIND_CONSTRUCT = 0b1,
  ENTITY_KIND_CAMERA = 0b10,
  ENTITY_KIND_count = 3,
} entity_kind_t;

typedef entity_kind_t entity_kind_mask_t;

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

static void entity_init_camera(entity_t *entity, allocator_t alloc,
                               vec2i chunk_relative, vec2 chunk_local,
                               float scale) {
  entity->kind = ENTITY_KIND_CAMERA;
  entity->chunk_relative_position = chunk_relative;
  entity->chunk_local_position = chunk_local;

  entity->as_camera.scale = scale;
}

static mat4 entity_camera_calculate_projection(const rendering_ctx_t *ctx,
                                               const entity_t *entity) {
  ASSERT__(entity->kind == ENTITY_KIND_CAMERA);

  float scale = entity->as_camera.scale;
  float ar = scale * (float)ctx->width / (float)ctx->height;
  float left = -ar, right = ar;
  float bottom = -scale, top = scale;

  vec2 v = entity->chunk_local_position;

  mat4 mat = mat4_zero();
  float data[16] = {
      2.f / (right - left),
      0,
      0,
      0,
      0,
      2.f / (top - bottom),
      0,
      0,
      0,
      0,
      -1,
      0,
      (-(right + left) - 2.f * v.x) / (right - left),
      (-(top + bottom) - 2.f * v.y) / (top - bottom),
      0,
      1,
  };

  memcpy(&mat.data, data, sizeof data);

  return mat;
}

#endif
