#include "shader.h"

#include <GL/glew.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    const char *shaderTypeName = shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment";
    fprintf(stderr, "Failed to compile %s shader! Error: %s\n", shaderTypeName,
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

shaderId compileAndLinkShader(const char *vertexPath, const char *fragPath) {
  FILE *vertexFile = fopen(vertexPath, "r");
  if (!vertexFile) {
    fprintf(stderr, "Failed to open vertex shader source file at path '%s'\n",
            vertexPath);
    exit(EXIT_FAILURE);
  }
  FILE *fragFile = fopen(fragPath, "r");
  if (!fragFile) {
    fprintf(stderr, "Failed to open fragment shader source file at path '%s'\n",
            fragPath);
    exit(EXIT_FAILURE);
  }
  char srcBuf[8192];
  memset(srcBuf, 0, 8192);
  fread(srcBuf, 1, 8192, vertexFile);
  fclose(vertexFile);
  unsigned int vert = compileShader(GL_VERTEX_SHADER, srcBuf);
  memset(srcBuf, 0, 8192);
  fread(srcBuf, 1, 8192, fragFile);
  fclose(fragFile);
  unsigned int frag = compileShader(GL_FRAGMENT_SHADER, srcBuf);
  shaderId program = linkShaders(2, (unsigned int[2]){vert, frag});
  glDeleteShader(vert);
  glDeleteShader(frag);
  return program;
}

void useShader(shaderId Id) {
  glUseProgram(Id);
}

void setInt(shaderId Id, const char *name, int value) {
  glUniform1i(glGetUniformLocation(Id, name), value);
}

void setFloat(shaderId Id, const char *name, float value) {
  glUniform1f(glGetUniformLocation(Id, name), value);
}

void setMat4(shaderId Id, const char *name, float *value) {
  glUniformMatrix4fv (glGetUniformLocation(Id, name), 1, GL_FALSE, value);
}
