#ifndef __GLYPH_H__
#define __GLYPH_H__

typedef struct ROLGlyph {
  unsigned int width;
  unsigned int height;
  int off_x;
  int off_y;
  unsigned char *bitmap;
} ROLGlyph;

#endif // __GLYPH_H__
