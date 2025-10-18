#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW error: %s\n", description);
}

int main() {
  // Set error callback
  glfwSetErrorCallback(error_callback);

  // Initialize GLFW and handle error
  if (!glfwInit()) {
    fprintf(stderr, "failed to initialize GLFW!\n");
    exit(EXIT_FAILURE);
  }

  // Create window
  GLFWwindow *window = glfwCreateWindow(
      640, 480, "rol-font-renderer - GNU Unifont example", NULL, NULL);
  if (!window) {
    fprintf(stderr, "failed to create GLFW window!\n");
    exit(EXIT_FAILURE);
  }
  // Make OpenGL context current
  glfwMakeContextCurrent(window);

  int width;
  int height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glfwSwapInterval(1);

  // Main loop
  while (!glfwWindowShouldClose(window)) {

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
