#include "GL/glew.h"
#include "display/texture.h"
#include "errors.h"
#include "font_loader.h"
#include "glyph.h"
#include "shader.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Quad {
  float pos_x;
  float pos_y;
  float width;
  float height;
  unsigned int texture;
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;
  float *data;
  unsigned int *indices_data;
};

void quad_init(struct Quad *quad) {
  // Create a VAO
  glGenVertexArrays(1, &quad->vao);
  glBindVertexArray(quad->vao);

  size_t data_size = sizeof(float) * (3 * 4 + 4 * 2);
  // Initialize buffer data
  quad->data = malloc(data_size);

  glGenBuffers(1, &quad->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
  glBufferData(GL_ARRAY_BUFFER, data_size, quad->data, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  unsigned int indices[] = {0, 1, 3, 0, 3, 2};
  glGenBuffers(1, &quad->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_DYNAMIC_DRAW);

  // Unbind the VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void quad_draw(struct Quad *quad) {
  // Bind VAO
  glBindVertexArray(quad->vao);
  glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->ebo);

  // Build buffer data with the coordinates
  // clang-format off
  float rectangle[] = {
    quad->pos_x, quad->pos_y, 0.0f,
    0.0f, 0.0f,
    quad->pos_x + quad->width, quad->pos_y, 0.0f,
    1.0f, 0.0f,
    quad->pos_x, quad->pos_y + quad->height, 0.0f,
    0.0f, 1.0f,
    quad->pos_x + quad->width, quad->pos_y + quad->height, 0.0f,
    1.0f, 1.0f
  };

  size_t data_size = sizeof(float) * (3 * 4 + 4 * 2);
  memcpy(quad->data, rectangle, data_size);
  // clang-format on
  // Update buffer data
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, quad->data);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, quad->texture);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // Unbind VAO
  glBindVertexArray(0);
}


static float y_off = 0.0f;
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  y_off -= y_offset * 20.0f;
}

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW error: %s\n", description);
}

void updateViewport(GLFWwindow *window) {
  int width;
  int height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <font_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  const char *font_file = argv[1];
  // Load BDF font
  ROLFont *font;
  ROLFRError err =
      load_font_from_file(BDF, argv[1], &font);
  if (err != SUCCESS) {
    fprintf(stderr, "Failed to load BDF font, error: %s\n",
            get_rolfr_error_string(err));
    exit(EXIT_FAILURE);
  }
  // Print some info about the font
  printf("font name: %s\n", font->name);
  printf("font point size: %u\n", font->size[0]);
  printf("font x resolution: %u, y resolution: %u\n", font->size[1],
         font->size[2]);
  printf("bounding box: x: %d, y: %d. offset: x: %d, y: %d\n",
         font->font_bouding_box[0], font->font_bouding_box[1],
         font->font_bouding_box[2], font->font_bouding_box[3]);
  printf("font number of glyphs: %zu\n", font->n_glyphs);

  // Set error callback
  glfwSetErrorCallback(error_callback);

  // Initialize GLFW and handle error
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW!\n");
    exit(EXIT_FAILURE);
  }

  // Create window
  GLFWwindow *window = glfwCreateWindow(
      640, 480, "rol-font-renderer - OpenGL example", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window!\n");
    exit(EXIT_FAILURE);
  }
  // Handle scroll
  glfwSetScrollCallback(window, scroll_callback);
  // Make OpenGL context current
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();

  glfwSwapInterval(0);

  // Set byte alignment for texture
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Shader compilation
  shaderId shaderProgram =
      compileAndLinkShader("examples/opengl/shaders/vertex.glsl",
                           "examples/opengl/shaders/frag.glsl");
  useShader(shaderProgram);

  // Build texture for all glyphs!
  unsigned int *glyph_textures = malloc(sizeof(unsigned int) * font->n_glyphs);
  glGenTextures(font->n_glyphs, glyph_textures);
  for (size_t i = 0; i < font->n_glyphs; ++i) {
    ROLGlyph glyph;
    if ((err = get_glyph(font, &font->glyphs[i], &glyph)) != SUCCESS) {
      fprintf(stderr, "An error occurred while loading a glyph: %s\n",
              get_rolfr_error_string(err));
      exit(EXIT_FAILURE);
    }
    unsigned char *texture_data;
    if ((err = build_texture(&glyph, &texture_data)) != SUCCESS) {
      fprintf(stderr,
              "An error occured while building the texture for a glyph: %s\n",
              get_rolfr_error_string(err));
      exit(EXIT_FAILURE);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glyph_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0, GL_RED,
                 GL_UNSIGNED_BYTE, texture_data);
    //    free(texture_data);
  }

  struct Quad quad;
  quad_init(&quad);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Update viewport size and clear
    updateViewport(window);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    useShader(shaderProgram);
    // Set projection matrix
    mat4 proj;
    glm_ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f, proj);
    setMat4(shaderProgram, "projection", (float *)proj);
    //    quad_draw(&quad);

    float scale = 1.;
    float xpos = 0.0f, ypos = 0.0f;
    for (size_t i = 0; i < font->n_glyphs; ++i) {
      if (ypos + y_off > height) {
        break;
      }
      ROLGlyph glyph;
      if ((err = get_glyph(font, &font->glyphs[i], &glyph)) != SUCCESS) {
        fprintf(stderr, "An error occured while loading a glyph: %s\n",
                get_rolfr_error_string(err));
        exit(EXIT_FAILURE);
      }

      quad.pos_x = xpos;
      quad.pos_y = ypos + y_off;
      quad.width = (float)glyph.width;
      quad.height = (float)glyph.height;
      quad.texture = glyph_textures[i];

      if (ypos + y_off >= 0) {
        quad_draw(&quad);
      }

      xpos += glyph.width * scale + 1;
      if (xpos >= width) {
        xpos = 0.0f;
        ypos += font->font_bouding_box[1];
      }
    }
    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  free(glyph_textures);
  free_font(BDF, font);

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
