#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "  fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW error: %s\n", description);
}

unsigned int compileShader(int shaderType, const char *shaderSource) {
  unsigned int shader;
  shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Failed to compile type %d shader! Error: %s\n", shaderType,
            infoLog);
    exit(EXIT_FAILURE);
  }
  return shader;
}

unsigned int linkShaders(size_t nShaders, unsigned int shaders[nShaders]) {
  unsigned int shaderProgram = glCreateProgram();
  for (size_t i = 0; i < nShaders; ++i) {
    glAttachShader(shaderProgram, shaders[i]);
  }
  glLinkProgram(shaderProgram);
  int success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Failed to link type shader program! Error: %s\n", infoLog);
    exit(EXIT_FAILURE);
  }
  return shaderProgram;
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
  unsigned int vertexShader =
      compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  unsigned int fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  unsigned int shaderProgram =
      linkShaders(2, (unsigned int[2]){vertexShader, fragmentShader});
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(shaderProgram);

  // Triangle data
  float triangle[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Update viewport size and clear
    updateViewport(window);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shaders and VAO and draw triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
