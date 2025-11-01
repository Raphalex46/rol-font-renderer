#include "display/texture.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>

#define MAX(a, b) a > b ? a : b

static inline size_t get_chunk_id(size_t glyph_id, size_t glyphs_per_atlas) {
  return glyph_id / glyphs_per_atlas;
}

size_t get_off_x(ROLTextureCtx *ctx, size_t local_id) {
  size_t row_size = TEXTURE_CHUNK_SIZE / ctx->width_per_glyph;
  return (local_id % row_size) * ctx->width_per_glyph;
}

size_t get_off_y(ROLTextureCtx *ctx, size_t local_id) {
  size_t row_size = TEXTURE_CHUNK_SIZE / ctx->width_per_glyph;
  return (local_id / row_size) * ctx->width_per_glyph;
}

ROLTexture get_texture_info(ROLTextureCtx *ctx, ROLGlyph *glyph,
                            size_t chunk_id) {
  size_t first_chunk_glyph = chunk_id * TEXTURE_CHUNK_SIZE;
  size_t local_id = glyph->id - first_chunk_glyph;
  ROLTexture texture = {.off_x = get_off_x(ctx, local_id),
                        .off_y = get_off_y(ctx, local_id),
                        .width = glyph->width,
                        .height = glyph->height,
                        .texture_data = ctx->atlases[chunk_id]};
  return texture;
}

static inline void write_to_texture_atlas(ROLTextureAtlas atlas, size_t x,
                                          size_t y, unsigned char byte) {
  y = TEXTURE_CHUNK_SIZE - y - 1;
  atlas[y * TEXTURE_CHUNK_SIZE + x] = byte;
}

void fill_texture_atlas(ROLTextureCtx *ctx, size_t chunk_id) {
  ctx->atlases[chunk_id] = calloc(1, TEXTURE_CHUNK_SIZE * TEXTURE_CHUNK_SIZE);
  size_t start = chunk_id * ctx->glyph_per_atlas;
  for (size_t i = start; i < start + ctx->glyph_per_atlas; ++i) {
    size_t local_id = i - start;
    size_t off_x = get_off_x(ctx, local_id);
    size_t off_y = get_off_y(ctx, local_id);
    size_t width = ctx->font->glyphs[i].bbx[0];
    size_t height = ctx->font->glyphs[i].bbx[1];
    size_t n_bytes = ((width - 1) >> 3) + 1;
    unsigned char *bitmap_ptr = ctx->font->glyphs[i].bitmap;
    for (size_t j = off_y; j < off_y + height; ++j) {
      for (size_t k = 0; k < n_bytes; ++k) {
        for (size_t in_b = 0; in_b < 8 && k * 8 + in_b < width; ++in_b) {
          unsigned char byte = (((*bitmap_ptr) >> (7 - in_b)) & 1) ? 255 : 0;
          write_to_texture_atlas(ctx->atlases[chunk_id], off_x + k * 8 + in_b,
                                 j, byte);
        }
        ++bitmap_ptr;
      }
    }
  }
}

ROLFRError init_texture_ctx(ROLTextureCtx **ctx, ROLFont *font) {
  size_t glyphs_per_atlas = (TEXTURE_CHUNK_SIZE / font->font_bouding_box[1]) *
                            (TEXTURE_CHUNK_SIZE / font->font_bouding_box[0]);
  size_t needed_chunks = font->n_glyphs / glyphs_per_atlas;
  *ctx = malloc(sizeof(ROLTextureCtx));
  **ctx =
      (ROLTextureCtx){.atlases = calloc(needed_chunks, sizeof(ROLTextureAtlas)),
                      .glyph_per_atlas = glyphs_per_atlas,
                      .width_per_glyph = font->font_bouding_box[0],
                      .height_per_glyph = font->font_bouding_box[1],
                      .font = font};
  return SUCCESS;
}

// void fill_texture_atlas(ROLTextureAtlas atlas, ROLTextureCtx *ctx,
//                         size_t chunk_id) {
//   // Allocate a texture big enough for our glyph data.
//   size_t x = 0;
//   size_t y = 0;
//   for (size_t i = 0; i < ctx->font->n_glyphs; ++i) {
//     if (ctx->texture_glyphs[i].chunk_id == chunk_id) {
//     }
//   }
//   unsigned char *texture = malloc(glyph->height * glyph->width);
//   size_t texture_i = 0;
//   unsigned char *bitmap_ptr = glyph->bitmap;
//   size_t n_bytes = ((glyph->width - 1) >> 3) + 1;
//   for (int i = glyph->height; i > 0; --i) {
//     texture_i = glyph->width * (i - 1);
//     for (size_t j = 0; j < n_bytes; ++j) {
//       for (size_t k = 0; k < 8 && (j * 8) + k < glyph->width; ++k) {
//         texture[texture_i++] = (((*bitmap_ptr) >> (7 - k)) & 1) ? 255 : 0;
//       }
//       bitmap_ptr++;
//     }
//   }
//   *texture_data = texture;
//   return SUCCESS;
//}

ROLFRError get_texture(ROLTextureCtx *ctx, ROLGlyph *glyph,
                       ROLTexture *texture) {
  size_t chunk_id = get_chunk_id(glyph->id, ctx->glyph_per_atlas);
  if (ctx->atlases[chunk_id]) {
    // Retrieve texture info
    *texture = get_texture_info(ctx, glyph, chunk_id);
    return SUCCESS;
  } else {
    fill_texture_atlas(ctx, chunk_id);
    *texture = get_texture_info(ctx, glyph, chunk_id);
    return SUCCESS;
  }
}
