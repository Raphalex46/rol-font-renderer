#include "display/opengl/opengl.h"
#include "display/texture.h"
#include <GL/glew.h>

void create_opengl_texture(ROLOpenGLTextureCtx *ctx, size_t chunk_id,
                           unsigned char *texture_data) {
  glGenTextures(1, &ctx->atlases[chunk_id]);
  glBindTexture(GL_TEXTURE_2D, ctx->atlases[chunk_id]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TEXTURE_CHUNK_SIZE, TEXTURE_CHUNK_SIZE,
               0, GL_RED, GL_UNSIGNED_BYTE, texture_data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

ROLFRError get_opengl_glyph(ROLOpenGLTextureCtx *ctx, ROLGlyph *glyph,
                            ROLOpenGLGlyph *opengl_glyph) {
  ROLTexture texture;
  ROLFRError err;
  if ((err = get_texture(ctx->tex_ctx, glyph, &texture)) != SUCCESS) {
    return err;
  }
  *opengl_glyph = (ROLOpenGLGlyph){
      .tex_off_x = texture.off_x / (float)TEXTURE_CHUNK_SIZE,
      .tex_off_y = texture.off_y / (float)TEXTURE_CHUNK_SIZE,
      .tex_width = texture.width / (float)TEXTURE_CHUNK_SIZE,
      .tex_height = texture.height / (float)TEXTURE_CHUNK_SIZE,
  };
  if (!ctx->atlases[texture.chunk_id]) {
    create_opengl_texture(ctx, texture.chunk_id, texture.texture_data);
  }

  opengl_glyph->atlas = ctx->atlases[texture.chunk_id];
  return SUCCESS;
}

ROLFRError init_opengl_texture_ctx(ROLOpenGLTextureCtx **ctx, ROLFont *font) {
  *ctx = malloc(sizeof(ROLOpenGLTextureCtx));
  ROLFRError err;
  if ((err = init_texture_ctx(&(*ctx)->tex_ctx, font)) != SUCCESS) {
    free(*ctx);
    return err;
  }
  (*ctx)->atlases =
      calloc(sizeof(unsigned int), get_num_chunks((*ctx)->tex_ctx));
  return SUCCESS;
}

void free_opengl_texture_ctx(ROLOpenGLTextureCtx *ctx) {
  free(ctx->atlases);
  free_texture_ctx(ctx->tex_ctx);
  free(ctx);
};
