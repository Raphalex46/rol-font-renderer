#include "display/texture.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>

ROLFRError build_texture(ROLGlyph *glyph, unsigned char **texture_data) {
  // Allocate a texture big enough for our glyph data.
  unsigned char *texture = malloc(glyph->height * glyph->width);
  size_t texture_i = 0;
  unsigned char *bitmap_ptr = glyph->bitmap;
  size_t n_bytes = ((glyph->width - 1) >> 3) + 1;
  for (int i = glyph->height; i > 0; --i) {
    texture_i = glyph->width * (i - 1);
    for (size_t j = 0; j < n_bytes; ++j) {
      for (size_t k = 0; k < 8 && (j * 8) + k < glyph->width; ++k) {
        texture[texture_i++] = (((*bitmap_ptr) >> (7 - k)) & 1) ? 255 : 0;
      }
      bitmap_ptr++;
    }
  }
  *texture_data = texture;
  return SUCCESS;
}
