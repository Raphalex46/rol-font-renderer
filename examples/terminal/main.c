#include "display/string.h"
#include "font_loader.h"
#include "glyph.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Load font
  ROLFont *font;
  load_font_from_file(BDF, "examples/common/fonts/bdf/unifont.bdf", &font);
  for (size_t i = 0; i < font->n_glyphs; ++i) {
    char *str;
    ROLGlyph glyph;
    get_glyph(font, font->glyphs[i].encoding, &glyph);
    string_display(&glyph, &str);
    printf("character %u:\n", font->glyphs[i].encoding);
    printf("%s\n", str);
    free(str);
  }
  free_font(BDF, font);
}
