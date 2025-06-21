#ifndef __SPACED_H__RENDER_QUADS__
#define __SPACED_H__RENDER_QUADS__

#include "instances.h"

static GLuint _gl_quad_vbo, _gl_quad_vao;

static void gl_quad_init() {
  static float quad[] = {-.5, -.5, .5, -.5, -.5, .5, .5, .5};

  glGenBuffers(1, &_gl_quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _gl_quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  
  glGenVertexArrays(1, &_gl_quad_vao);
  glBindVertexArray(_gl_quad_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
}

typedef struct {
  instance_buffer_t instances;
} render_quads_t;

static void render_quads_init(render_quads_t *quads, allocator_t alloc,
                              sz capacity) {
  instance_buffer_init(&quads->instances, alloc, capacity);
}

#endif
