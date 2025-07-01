#ifndef __H__ENGINE_CONSTRUCT__
#define __H__ENGINE_CONSTRUCT__

#include "../6502/chip.h"
#include "../list.h"
#include "../rendering/instances.h"
#include "../vec2i.h"

typedef enum {
  BLOCK_KIND_MESH,
  BLOCK_KIND_THRUSTER,
  BLOCK_KIND_ENGINE,
  BLOCK_KIND_CHIP,
  BLOCK_KIND_count,
} block_kind_t;

static int block_kind_to_tile_index(block_kind_t kind) {
  switch (kind) {
  case BLOCK_KIND_MESH:
    return 0;
  case BLOCK_KIND_CHIP:
    return 4;
  }

  return -1;
}

typedef struct {
  block_kind_t kind;
  vec2i offset;
  union {
    struct {
      chip_t chip;
    } as_chip;
  };
} block_t;

static block_t block_new_mesh() {
  block_t ret;
  ret.kind = BLOCK_KIND_MESH;
  return ret;
}

static block_t block_new_chip(allocator_t alloc, sz memory_size) {
  block_t ret;
  ret.kind = BLOCK_KIND_CHIP;

  chip_init(&ret.as_chip.chip, alloc, memory_size, 0);

  return ret;
}

typedef struct {
  list_t blocks;
  instance_buffer_t instance;
  bool is_dirty;
} construct_t;

#endif
