#include "display/string.h"
#include "font_loader.h"
#include "glyph.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <font_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  const char *font_file = argv[1];
  // Load font
  ROLFont *font;
  load_font_from_file(BDF, font_file, &font);
  for (size_t i = 0; i < font->n_glyphs; ++i) {
    char *str;
    ROLGlyph glyph;
    get_glyph(font, &font->glyphs[i], &glyph);
    string_display(&glyph, &str);
    printf("character %u:\n", font->glyphs[i].encoding);
    printf("%s\n", str);
    free(str);
  }
  free_font(BDF, font);
}
