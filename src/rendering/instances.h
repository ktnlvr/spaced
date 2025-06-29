#ifndef __SPACED_H__RENDERING_INSTANCES__
#define __SPACED_H__RENDERING_INSTANCES__

#include <GL/glew.h>

#include "../list.h"
#include "../memory.h"
#include "../vec2.h"

typedef struct instance_t {
  u32 tile_index;
  vec2 position;
} instance_t;

static void instance_init(instance_t *instance, vec2 position, u32 tile_index) {
  instance->position = position;
  instance->tile_index = tile_index;
}

typedef struct instance_buffer_t {
  GLuint vbo;
  instance_t *instances;
  sz capacity;
  sz size;

  list_t write_indices;
} instance_buffer_t;

static void instance_buffer_init(instance_buffer_t *buffer, allocator_t alloc,
                                 sz capacity) {
  buffer->capacity = capacity;
  buffer->instances = allocator_alloc_ty(instance_t, alloc, buffer->capacity);
  memset(buffer->instances, 0, buffer->capacity * sizeof(instance_t));

  buffer->size = 0;

  list_init_ty(u32, &buffer->write_indices, alloc);

  glGenBuffers(1, &buffer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(instance_t) * buffer->capacity,
               buffer->instances, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(instance_t),
                        (void *)0);
  glVertexAttribDivisor(1, 1);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(instance_t),
                        (void *)4);
  glVertexAttribDivisor(2, 1);

  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void instance_buffer_push(instance_buffer_t *buffer,
                                 instance_t instance) {
  ASSERT__(buffer->size != buffer->capacity);

  sz at = buffer->size++;
  buffer->instances[at] = instance;
  list_push(&buffer->write_indices, &at);
}

static void instance_buffer_clear(instance_buffer_t *buffer) {
  buffer->size = 0;
  list_clear(&buffer->write_indices);
}

static void instance_buffer_flush(instance_buffer_t *buffer) {
  glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);

  for (sz i = 0; i < buffer->write_indices.size; i++) {
    u32 idx = list_get_ty(u32, &buffer->write_indices, i);
    GLsizeiptr offset = idx * sizeof(instance_t);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_t),
                    &buffer->instances[i]);
  }
}

#endif
