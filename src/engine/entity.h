#ifndef __SPACED_H__ENGINE_ENTITY__
#define __SPACED_H__ENGINE_ENTITY__

#include "../defs.h"
#include "../vec2.h"

typedef enum {
  ENTITY_KIND_CONSTRUCT,
  ENTITY_KIND_RENDER_QUADS,
  ENTITY_KIND_CAMERA,
  ENTITY_KIND_count,
} entity_kind_t;

typedef struct entity_t {
  struct entity_t *next;
  u32 canary;
  entity_kind_t kind;
  vec2 chunk_position;
  // The AABB extents of the entity
  vec2 extents;
} entity_t;

typedef entity_t *entity_ptr_t;

#endif
