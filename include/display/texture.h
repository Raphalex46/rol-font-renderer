#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "errors.h"
#include "font_loader.h"
#include "glyph.h"
#include <stddef.h>

#define TEXTURE_CHUNK_SIZE 256u

typedef unsigned char *ROLTextureAtlas;

typedef struct {
  unsigned char *texture_data;
  size_t off_x;
  size_t off_y;
  size_t width;
  size_t height;
  size_t chunk_id;
} ROLTexture;

typedef struct {
  ROLTextureAtlas *atlases;
  size_t glyph_per_atlas;
  unsigned int width_per_glyph;
  unsigned int height_per_glyph;
  ROLFont *font;
} ROLTextureCtx;

ROLFRError init_texture_ctx(ROLTextureCtx **ctx, ROLFont *font);

/// \brief Generate a texture from a glyph.
ROLFRError get_texture(ROLTextureCtx *ctx, ROLGlyph *glyph,
                       ROLTexture *texture);

size_t get_num_chunks(ROLTextureCtx *ctx);

#endif // __TEXTURE_H__
