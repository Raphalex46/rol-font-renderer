#include "GL/glew.h"
#include "display/texture.h"
#include "errors.h"
#include "font_loader.h"
#include "glyph.h"
#include "shader.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

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
  // Load BDF font
  ROLFont *font;
  ROLFRError err =
      load_font_from_file(BDF, "examples/common/fonts/bdf/unifont.bdf", &font);
  if (err != SUCCESS) {
    fprintf(stderr, "Failed to load BDF font, error: %s\n",
            get_rolfr_error_string(err));
    exit(EXIT_FAILURE);
  }
  // Print some info about the font
//  printf("font name: %s\n", font->name);
//  printf("font point size: %u\n", font->size[0]);
//  printf("font x resolution: %u, y resolution: %u\n", font->size[1],
//         font->size[2]);
//  printf("bounding box: x: %d, y: %d. offset: x: %d, y: %d\n",
//         font->font_bouding_box[0], font->font_bouding_box[1],
//         font->font_bouding_box[2], font->font_bouding_box[3]);
//  printf("font number of glyphs: %zu\n", font->n_glyphs);
//  for (size_t i = 0; i < font->n_glyphs; ++i) {
//    printf("glyph %s:\n", font->glyphs[i].name);
//    printf("\t encoding: %d\n", font->glyphs[i].encoding);
//    printf("\t swidth: x: %f, y: %f\n", font->glyphs[i].swidth[0],
//           font->glyphs[i].swidth[1]);
//    printf("\t dwidth: x: %u, y: %u\n", font->glyphs[i].dwidth[0],
//           font->glyphs[i].dwidth[1]);
//    printf("\t bounding box: w: %d, h: %d, off_x: %d, off_y: %d\n",
//           font->glyphs[i].bbx[0], font->glyphs[i].bbx[1],
//           font->glyphs[i].bbx[2], font->glyphs[i].bbx[3]);
//  }

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

  // Set byte alignment for texture
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Shader compilation
  shaderId shaderProgram =
      compileAndLinkShader("examples/common/shaders/vertex.glsl",
                           "examples/common/shaders/frag.glsl");
  useShader(shaderProgram);

  // Rectangle data
  // clang-format off
  float rectangle[] =
    {
      -1.0f, -1.0f, 0.0f,
      0.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f, 0.0f,
      1.0f, 1.0f
    };
  // clang-format on

  unsigned int indices[] = {0, 1, 3, 0, 3, 2};

  // clang-format off
//  unsigned char textureData[] =
//  {
//    0, 0, 0,
//    255, 255, 255,
//    255, 255, 255,
//    0, 0, 0
//  };
  // clang-format on

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  unsigned int encoding = 0;
  double old_time = glfwGetTime();
  double delta = 0.;
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Update viewport size and clear
    updateViewport(window);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ROLGlyph glyph;
    if (delta >= 0.5) {
      encoding++;
      delta = 0.;
    }
    if ((err = get_glyph(font, encoding, &glyph)) != SUCCESS) {
      fprintf(stderr, "An error occured while loading a glyph: %s\n",
          get_rolfr_error_string(err));
    }
    unsigned char *textureData;
    if ((err = build_texture(&glyph, &textureData)) != SUCCESS) {
      fprintf(stderr,
          "An error occured while building the texture for a glyph: %s\n",
          get_rolfr_error_string(err));
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0, GL_RED, GL_UNSIGNED_BYTE,
               textureData);
    delta += glfwGetTime() - old_time;
    old_time = glfwGetTime();

    // Set shaders and VAO and draw triangle
    useShader(shaderProgram);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwWaitEvents();
    free(textureData);
  }

  free_font(BDF, font);

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
