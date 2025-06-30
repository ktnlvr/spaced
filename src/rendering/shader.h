#ifndef __H__ENGINE_SHADER__
#define __H__ENGINE_SHADER__

#include <GL/glew.h>

#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "../defs.h"
#include "../memory.h"

/// @brief Small container and wrapper over OpenGL for
/// shader-related things. In actually it is closer to
/// what OpenGL calls a "program".
typedef struct {
  GLuint gl_program;
} shader_t;

static GLuint shader__compile_opengl_shader_or_panic(const char *src,
                                                     GLuint kind) {
  allocator_t alloc = allocator_new_stack_alloc();

  GLuint shader = glCreateShader(kind);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  GLint is_ok = GL_TRUE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_ok);
  if (is_ok != GL_TRUE) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    char *out = allocator_alloc_ty(char, alloc, length);
    glGetShaderInfoLog(shader, length, 0, out);
    PANIC("Shader Compilation Failed: %s", out);
  }

  return shader;
}

/// @memberof shader_t
static void shader_init(shader_t *shader, const char *vertex_src,
                        const char *fragment_src) {
  allocator_t alloc = allocator_new_stack_alloc();

  ASSERT__(vertex_src);
  ASSERT__(fragment_src);

  GLuint vs =
      shader__compile_opengl_shader_or_panic(vertex_src, GL_VERTEX_SHADER);
  GLuint fs =
      shader__compile_opengl_shader_or_panic(fragment_src, GL_FRAGMENT_SHADER);

  shader->gl_program = glCreateProgram();
  glAttachShader(shader->gl_program, vs);
  glAttachShader(shader->gl_program, fs);

  glLinkProgram(shader->gl_program);

  GLint is_ok = GL_TRUE;
  glGetShaderiv(shader->gl_program, GL_LINK_STATUS, &is_ok);
  if (is_ok != GL_TRUE) {
    GLint length = 0;
    glGetShaderiv(shader->gl_program, GL_INFO_LOG_LENGTH, &length);

    char *out = allocator_alloc_ty(char, alloc, length);
    glGetShaderInfoLog(shader->gl_program, length, 0, out);
    PANIC("Shader Linking Failed: %s", out);
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
}

static void shader_cleanup(shader_t *shader) {
  glDeleteProgram(shader->gl_program);
}

#endif
