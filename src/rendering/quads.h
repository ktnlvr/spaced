#ifndef __H__RENDERING_QUADS__
#define __H__RENDERING_QUADS__

#include "instances.h"

static GLuint _gl_quad_vbo, _gl_quad_vao;

static void gl_quad_init() {
  static float quad[] = {-.5, -.5, 0., 0., .5, -.5, 1., 0.,
                         -.5, .5,  0., 1., .5, .5,  1., 1.};

  glGenVertexArrays(1, &_gl_quad_vao);
  glBindVertexArray(_gl_quad_vao);

  glGenBuffers(1, &_gl_quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _gl_quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(sizeof(float) * 2));
}

typedef struct {
  instance_buffer_t instances;
} render_quads_t;

static void render_quads_init(render_quads_t *quads, allocator_t alloc,
                              sz capacity) {
  instance_buffer_init(&quads->instances, alloc, capacity);
}

#endif
