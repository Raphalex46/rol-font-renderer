#ifndef __BDF_H__
#define __BDF_H__

/// \file Module for loading BDF font files.
///
/// See the BDF specification:
/// <https://adobe-type-tools.github.io/font-tech-notes/pdfs/5005.BDF_Spec.pdf>.
///
/// This parser limits itself to BDF version 2.1 files.

#include "errors.h"
#include "font_loader.h"
#include <stddef.h>

#define BDF_LINE_MAX

struct BDFGlyph {
  char name[15]; ///< Glyph name.
  unsigned int encoding; ///< Encoding.
  float swidth[2]; ///< Unimplemented.
  unsigned int dwidth[2]; ///< device width in device pixels.
  unsigned int bbx[4]; ///< BBw, BBh, BBxoff0x, BByoff0y.
  char *bitmap; ///< Pointer to the bitmap data.
};

struct BDFFont {
  char * name; ///< Unimplemented.
  unsigned int size[3]; ///< PointSize, Xres, Yres.
  int font_bouding_box[4]; ///< FBB, FBBy, Xoff, Yoff.
  size_t n_glyphs; ///< Number of glyphs in the font.
  struct BDFGlyph *glyphs; ///< List of glyphs.
  char *bitmaps; ///< A buffer containing all character bitmaps.
};

ROLFRError load_bdf_font_from_file(const char *path, struct BDFFont *result_font);

void free_bdf_font(struct BDFFont *font);

#endif // __BDF_H__
