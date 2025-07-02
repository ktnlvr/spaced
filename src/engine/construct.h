#ifndef __H__ENGINE_CONSTRUCT__
#define __H__ENGINE_CONSTRUCT__

#include "../6502/chip.h"
#include "../list.h"
#include "../map.h"
#include "../rendering/instances.h"
#include "../vec2i.h"

typedef enum {
  BLOCK_KIND_MESH,
  BLOCK_KIND_THRUSTER,
  BLOCK_KIND_ENGINE,
  BLOCK_KIND_CHIP,
  BLOCK_KIND_GYROSCOPE,
  BLOCK_KIND_CONTOLLER,
  BLOCK_KIND_ACCUMULATOR,
  BLOCK_KIND_count,
} block_kind_t;

static int block_kind_to_tile_index(block_kind_t kind) {
  switch (kind) {
  case BLOCK_KIND_MESH:
    return 1;
  case BLOCK_KIND_CHIP:
    return 2;
  case BLOCK_KIND_THRUSTER:
    return 3;
  }

  return -1;
}

typedef struct {
  block_kind_t kind;
  vec2i offset;
  int orientation;

  union {
    struct {
      chip_t chip;
    } as_chip;
    struct {
      byte throttle;
    } as_thruster;
  };
} block_t;

static block_t block_new_mesh() {
  block_t ret;
  ret.kind = BLOCK_KIND_MESH;
  return ret;
}
static block_t block_new_thruster(int orientation) {
  block_t ret;
  ret.kind = BLOCK_KIND_THRUSTER;
  ret.as_thruster.throttle = 0;

  return ret;
}

typedef struct construct_t {
  list_t blocks;
  instance_buffer_t instance;
  map_t device_map;
  bool is_dirty;
  vec2 velocity;
} construct_t;

static void block__external_call(chip_t *chip, block_t *block) {
  switch (block->kind) {
  case BLOCK_KIND_THRUSTER: {
    if (chip->y == 0) {
      byte throttle = chip_stack_pull(chip);
      block->as_thruster.throttle = throttle;
    }
    break;
  }
  }
  return;
}

static void block_chip__external_call(chip_t *chip) {
  construct_t *self = (construct_t *)chip->userdata;
  byte device = chip->x;
  vec2i *off = map_get_ty(vec2i, &self->device_map, device);

  if (off == 0) {
    chip->halted = true;
    return;
  }

  for (sz i = 0; i < self->blocks.size; i++) {
    block_t *blk = list_get_ty_ptr(block_t, &self->blocks, i);
    if (vec2_eq(blk->offset, *off)) {
      block__external_call(chip, blk);
      break;
    }
  }
}

static block_t block_new_chip(allocator_t alloc, sz memory_size) {
  block_t ret;
  ret.kind = BLOCK_KIND_CHIP;

  chip_init(&ret.as_chip.chip, alloc, memory_size, 0);
  ret.as_chip.chip.external_call = block_chip__external_call;

  return ret;
}

#endif
