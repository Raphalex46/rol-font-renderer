#include "font_loader.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>

ROLFRError load_font_from_file(enum FontFormat format, const char *path, ROLFont **result_font) {
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

ROLFRError get_glyph(ROLFont *font, unsigned int encoding, ROLGlyph *glyph) {
  for (size_t i = 0; i < font->n_glyphs; ++i) {
    if (font->glyphs[i].encoding == encoding) {
      glyph->bitmap = font->glyphs[i].bitmap;
      glyph->width = font->glyphs[i].bbx[0];
      glyph->height = font->glyphs[i].bbx[1];
      glyph->off_x = font->glyphs[i].bbx[2];
      glyph->off_y = font->glyphs[i].bbx[3];
    }
  }
  return UNDEFINED_GLYPH;
}
