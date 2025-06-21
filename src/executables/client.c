#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/world.h"
#include "../rendering/instances.h"

const char *vertex_src =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;"
    "layout (location = 1) in vec2 aOffset;"
    "uniform mat4 uProjection;"
    "void main() {"
    "    gl_Position = uProjection * vec4(aPos + aOffset, 0.0, 1.0);"
    "}";

const char *fragment_src = "#version 330 core\n"
                           "out vec4 FragColor;"
                           "void main() {"
                           "    FragColor = vec4(0.4, 0.7, 1.0, 1.0);"
                           "}";

static void set_projection(GLuint program, int width, int height) {
  float ar = (float)width / (float)height;
  float left = -ar, right = ar;
  float bottom = -1, top = 1;

  float ortho[16] = {2. / (right - left),
                     0,
                     0,
                     0,
                     0,
                     2. / (top - bottom),
                     0,
                     0,
                     0,
                     0,
                     -1,
                     0,
                     -(right + left) / (right - left),
                     -(top + bottom) / (top - bottom),
                     0,
                     1};

  GLint location = glGetUniformLocation(program, "uProjection");
  glUseProgram(program);
  glUniformMatrix4fv(location, 1, GL_FALSE, ortho);
}

int main(void) {
  allocator_t alloc = allocator_new_malloc();

  world_t world;
  world_init(&world);

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(640, 480, "Hello, world!", 0, 0);
  glfwMakeContextCurrent(window);

  glewInit();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_src, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_src, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  float quad[] = {-.5, -.5, .5, -.5, -.5, .5, .5, .5};

  GLuint VAO, VBO, instanceVBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VB
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

  instance_buffer_t instances;
  instance_buffer_init(&instances, alloc, 0x1000);

  render_quads_t *quads = world_spawn_render_quads(&world, 100);
  construct_t *cons = world_spawn_construct(&world, quads);

  component_t mesh;
  component_init_mesh(&mesh, vec2i_new(0, 1));

  construct_push_component(cons, mesh);

  instance_buffer_flush(&quads->instances);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    set_projection(program, width, height);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &instanceVBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(program);
  glfwDestroyWindow(window);
  glfwTerminate();

  world_cleanup(&world);

  return 0;
}
