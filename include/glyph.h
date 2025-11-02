#ifndef __GLYPH_H__
#define __GLYPH_H__

#include <stddef.h>

typedef struct ROLGlyph {
  size_t id;
  unsigned int width;
  unsigned int height;
  int off_x;
  int off_y;
  unsigned char *bitmap;
} ROLGlyph;

#endif // __GLYPH_H__
