#ifndef __H__RENDERING_IMAGE__
#define __H__RENDERING_IMAGE__

#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "../memory.h"

/// @brief The raw data of an image.
typedef struct {
  /// @brief The allocator that's used for the internal @ref raw buffer.
  allocator_t alloc;
  byte *raw;
  u32 width, height, channel;
  GLuint gl;
} image_t;

/// @brief Reinterprets the buffer as the contents of the image file and loads
/// the raw image into memory.
/// @memberof image_t
static void image_init(image_t *image, allocator_t alloc, const void *buffer,
                       sz size) {
  int width, height, channels;

  image->alloc = alloc;

  // TODO: decide if the texture should be premultiplied
  stbi_set_flip_vertically_on_load(1);
  void *temp = stbi_load_from_memory((const stbi_uc *)buffer, size, &width,
                                     &height, &channels, 4);

  image->raw =
      allocator_alloc_copy_ty(byte, alloc, temp, width * height * channels * 2);
  stbi_image_free(temp);

  image->width = width;
  image->height = height;
  image->channel = channels;

  glGenTextures(1, &image->gl);
  glBindTexture(GL_TEXTURE_2D, image->gl);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width, image->height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image->raw);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void image_bind(image_t *image) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image->gl);
}

/// @memberof image_t
static void image_cleanup(image_t *image) {
  allocator_free(image->alloc, image->raw);
}

#endif
