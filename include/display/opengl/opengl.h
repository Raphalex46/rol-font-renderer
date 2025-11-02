#ifndef __OPENGL_H__
#define __OPENGL_H__

#include "display/texture.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>
typedef struct {
  size_t atlas;
  float tex_off_x;
  float tex_off_y;
  float tex_width;
  float tex_height;
} ROLOpenGLGlyph;

typedef struct {
  ROLTextureCtx *tex_ctx;
  unsigned int *atlases;
} ROLOpenGLTextureCtx;

ROLFRError init_opengl_texture_ctx(ROLOpenGLTextureCtx **ctx, ROLFont *font);

ROLFRError get_opengl_glyph(ROLOpenGLTextureCtx *ctx, ROLGlyph *glyph,
                            ROLOpenGLGlyph *opengl_glyph);

void free_opengl_texture_ctx(ROLOpenGLTextureCtx *ctx);

#endif // __OPENGL_H__
