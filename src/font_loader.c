#include "font_loader.h"
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
