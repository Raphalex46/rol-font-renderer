#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "shader.h"

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW error: %s\n", description);
}

void updateViewport(GLFWwindow *window) {
  int width;
  int height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

int main() {
  // Set error callback
  glfwSetErrorCallback(error_callback);

  // Initialize GLFW and handle error
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW!\n");
    exit(EXIT_FAILURE);
  }

  // Create window
  GLFWwindow *window = glfwCreateWindow(
      640, 480, "rol-font-renderer - GNU Unifont example", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window!\n");
    exit(EXIT_FAILURE);
  }
  // Make OpenGL context current
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();

  glfwSwapInterval(1);

  // Shader compilation
  shaderId shaderProgram = compileAndLinkShader("../common/shaders/vertex.glsl", "../common/shaders/frag.glsl");
  useShader(shaderProgram);

  int ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
  // Rectangle data
  float rectangle[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 0.0f};

  unsigned int indices[] = {0, 1, 2, 0, 2, 3};

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Update viewport size and clear
    updateViewport(window);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shaders and VAO and draw triangle
    useShader(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
