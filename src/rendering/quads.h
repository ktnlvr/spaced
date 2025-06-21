#ifndef __SPACED_H__RENDER_QUADS__
#define __SPACED_H__RENDER_QUADS__

#include "instances.h"

typedef struct {
  instance_buffer_t instances;
} render_quads_t;

static void render_quads_init(render_quads_t *quads, allocator_t alloc,
                              sz capacity) {
  instance_buffer_init(&quads->instances, alloc, capacity);
}

#endif
