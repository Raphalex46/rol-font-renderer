#ifndef __FONT_LOADER_H__
#define __FONT_LOADER_H__

#include "glyph.h"
#include "loaders/bdf.h"
#include "errors.h"

typedef struct BDFFont ROLFont;

enum FontFormat { BDF };

ROLFRError load_font_from_file(enum FontFormat format, const char *path, ROLFont **result_font);

ROLFRError get_glyph(ROLFont *font, unsigned int encoding, ROLGlyph *glyph);

void free_font(enum FontFormat format, ROLFont *font);

#endif // __FONT_LOADER_H__
