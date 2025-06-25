#ifndef __SPACED_H__RENDERING_CAMERA__
#define __SPACED_H__RENDERING_CAMERA__

#include "../engine/world.h"
#include "../vec2.h"

#include "quads.h"

#define CAMERA_DEFAULT_SCALE 7

typedef struct {
  vec2 velocity;
  entity_ptr_t target;
  float scale;
} camera_t;

static void camera_init(camera_t *camera, entity_ptr_t target, float scale) {
  camera->velocity = vec2_zero();
  camera->target = 0;
}

DEFINE_ENTITY_PTR_AS(camera, ENTITY_KIND_CAMERA)

static camera_t *world_spawn_camera(world_t *world, entity_ptr_t target) {
  camera_t *camera =
      entity_ptr_as_camera(world_alloc_entity(world, ENTITY_KIND_CAMERA));

  camera_init(camera, target, CAMERA_DEFAULT_SCALE);

  return camera;
}

#endif
