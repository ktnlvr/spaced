#ifndef __H__RENDERING_CONTEXT__
#define __H__RENDERING_CONTEXT__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../defs.h"
#include "../vec2.h"
#include "../mat4.h"

typedef struct {
  i32 width, height;
  GLFWwindow *window;
} rendering_ctx_t;

static void glfwErrorCallback(int errc, const char *desc) {
  PANIC("GLFW Error %d: %s", errc, desc);
}

static void GLAPIENTRY glMessageCallback(GLenum source, GLenum type, GLuint id,
                                         GLenum severity, GLsizei length,
                                         const GLchar *message,
                                         const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

static void rendering_ctx_init(rendering_ctx_t *ctx) {
  glfwSetErrorCallback(glfwErrorCallback);

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  ctx->window = glfwCreateWindow(640, 480, "Hello, world!", 0, 0);
  glfwMakeContextCurrent(ctx->window);

  glewInit();

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glMessageCallback, 0);
}

static bool rendering_ctx_should_close(rendering_ctx_t *ctx) {
  return glfwWindowShouldClose(ctx->window);
}

static void rendering_ctx_frame_begin(rendering_ctx_t *ctx) {
  glfwGetFramebufferSize(ctx->window, &ctx->width, &ctx->height);
  glViewport(0, 0, ctx->width, ctx->height);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void rendering_ctx_set_projection(rendering_ctx_t *ctx, GLuint program,
                                         mat4 projection) {
  GLint location = glGetUniformLocation(program, "uProjection");
  glUseProgram(program);
  glUniformMatrix4fv(location, 1, GL_FALSE, projection.data);
}

static void rendering_ctx_frame_end(rendering_ctx_t *ctx) {
  glfwSwapBuffers(ctx->window);
  glfwPollEvents();
}

static void rendering_ctx_cleanup(rendering_ctx_t *ctx) {
  glfwDestroyWindow(ctx->window);
  glfwTerminate();
}

#endif
