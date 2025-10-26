#include "display/string.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>

ROLFRError string_display(ROLGlyph *glyph, char **out_string) {
  // Allocate enough space for the string, keeping in mind space for the null
  // terminator and newline characters
  char *str = malloc(glyph->height * (glyph->width + 1) + 1);
  unsigned char *bitmap_ptr = glyph->bitmap;
  size_t str_i = 0;
  for (size_t i = 0; i < glyph->height; ++i) {
    size_t charac_id = 0;
    for (size_t j = 0; j < ((glyph->width - 1) >> 3) + 1; ++j) {
      for (size_t k = 0; k < 8 && (j * 8) + k < glyph->width; ++k) {
        str[str_i++] = (((*bitmap_ptr) >> (7-k)) & 1) ? '#' : ' ';
      }
      bitmap_ptr++;
    }
    str[str_i++] = '\n';
  }
  str[str_i++] = 0;
  *out_string = str;
  return SUCCESS;
}
