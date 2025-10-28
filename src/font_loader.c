#include "font_loader.h"
#include "errors.h"
#include "glyph.h"
#include "loaders/bdf.h"
#include <stdlib.h>

ROLFRError load_font_from_file(enum FontFormat format, const char *path,
                               ROLFont **result_font) {
  *result_font = malloc(sizeof(ROLFont));
  switch (format) {
  case BDF:
    return load_bdf_font_from_file(path, *result_font);
    break;
  default:
    return UNKNOWN_FORMAT;
  }
}

void free_font(enum FontFormat format, ROLFont *font) {
  switch (format) {
  case BDF:
    free_bdf_font(font);
    break;
  }
}

ROLFRError get_glyph(ROLFont *font, struct BDFGlyph *glyph_bdf, ROLGlyph *glyph) {
  if (glyph) {
    glyph->bitmap = glyph_bdf->bitmap;
    glyph->width = glyph_bdf->bbx[0];
    glyph->height = glyph_bdf->bbx[1];
    glyph->off_x = glyph_bdf->bbx[2];
    glyph->off_y = glyph_bdf->bbx[3];

    return SUCCESS;
  } else {
    return UNKNOWN_FORMAT;
  }
}
