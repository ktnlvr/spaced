#ifndef __SPACED_H__ENGINE_CONSTRUCT__
#define __SPACED_H__ENGINE_CONSTRUCT__

#include "../rendering/quads.h"
#include "../vec2.h"

typedef struct construct_t {
  render_quads_t *quads;
} construct_t;

static void construct_init(construct_t *cons) {}

#endif
