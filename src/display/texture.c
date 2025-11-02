#include "display/texture.h"
#include "errors.h"
#include "glyph.h"
#include <stdlib.h>

static inline size_t get_chunk_id(size_t glyph_id, size_t glyphs_per_atlas) {
  return glyph_id / glyphs_per_atlas;
}

size_t get_off_x(ROLTextureCtx *ctx, size_t local_id) {
  size_t row_size = TEXTURE_CHUNK_SIZE / ctx->width_per_glyph;
  return (local_id % row_size) * ctx->width_per_glyph;
}

size_t get_off_y(ROLTextureCtx *ctx, size_t local_id) {
  size_t row_size = TEXTURE_CHUNK_SIZE / ctx->width_per_glyph;
  return (local_id / row_size) * ctx->height_per_glyph;
}

ROLTexture get_texture_info(ROLTextureCtx *ctx, ROLGlyph *glyph,
                            size_t chunk_id) {
  size_t first_chunk_glyph = chunk_id * TEXTURE_CHUNK_SIZE;
  size_t local_id = glyph->id - first_chunk_glyph;
  ROLTexture texture = {.off_x = get_off_x(ctx, local_id),
                        .off_y = get_off_y(ctx, local_id),
                        .width = glyph->width,
                        .height = glyph->height,
                        .texture_data = ctx->atlases[chunk_id],
                        .chunk_id = chunk_id
  };
  return texture;
}

static inline void write_to_texture_atlas(ROLTextureAtlas atlas, size_t x,
                                          size_t y, unsigned char byte) {
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
    unsigned char *bitmap_ptr = ctx->font->glyphs[i].bitmap + (height - 1) * n_bytes;
    for (size_t j = off_y; j < off_y + height; ++j) {
      for (size_t k = 0; k < n_bytes; ++k) {
        for (size_t in_b = 0; in_b < 8 && k * 8 + in_b < width; ++in_b) {
          unsigned char byte = (((*bitmap_ptr) >> (7 - in_b)) & 1) ? 255 : 0;
          write_to_texture_atlas(ctx->atlases[chunk_id], off_x + k * 8 + in_b,
                                 j, byte);
        }
        ++bitmap_ptr;
      }
      bitmap_ptr -= n_bytes * 2;
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
size_t get_num_chunks(ROLTextureCtx *ctx) {
  return ctx->font->n_glyphs / ctx->glyph_per_atlas;
}

