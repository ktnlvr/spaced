#include <GLFW/glfw3.h>

int main(void) {
  glfwInit();

  GLFWwindow *window = glfwCreateWindow(640, 480, "Hello, world!", 0, 0);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  return 0;
}
