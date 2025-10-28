#include "loaders/bdf.h"
#include "errors.h"
#include "font_loader.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_GETLINE(line, line_size, file)                                   \
  do {                                                                         \
    if ((getline(&line, &line_size, file) == -1)) {                            \
      if (line) {                                                              \
        free(line);                                                            \
      }                                                                        \
      if (ferror(file)) {                                                      \
        return FILE_READ;                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#define FORWARD_ERR(call)                                                      \
  do {                                                                         \
    ROLFRError err = call;                                                     \
    if (err != SUCCESS) {                                                      \
      return err;                                                              \
    }                                                                          \
  } while (0)

#define KEYWORD_CASE(keyword, token, parse_func)                               \
  do {                                                                         \
    if (!strcmp(keyword, token)) {                                             \
      FORWARD_ERR(parse_func);                                                 \
      return SUCCESS;                                                          \
    }                                                                          \
  } while (0)

enum ParserSection { GLOBAL, GLYPH, PROPERTIES, BITMAP };

struct ParserState {
  size_t current_line;
  size_t current_glyph;
  size_t current_bitmap_line;
  unsigned char *bitmap_ptr;
  enum ParserSection section;
};

ROLFRError remove_keyword(char **line, const char *keyword) {
  if (strlen(*line) < strlen(keyword)) {
    return PARSE_ERROR;
  }
  *line += strlen(keyword);
  return SUCCESS;
}

ROLFRError ignore_keyword(const char *line, struct BDFFont *font) {
  return SUCCESS;
}

ROLFRError parse_font_name(const char *line, struct BDFFont *font) {
  char *font_name = malloc(strlen(line) + 1);
  if (sscanf(line, "%*s %s", font_name) < 1) {
    free(font_name);
    return PARSE_ERROR;
  }
  font->name = font_name;
  return SUCCESS;
}

ROLFRError parse_font_size(const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %u %u %u", &font->size[0], &font->size[1],
             &font->size[2]) < 3) {
    return PARSE_ERROR;
  }
  return SUCCESS;
}

ROLFRError parse_font_bounding_box(const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %d %d %d %d", &font->font_bouding_box[0],
             &font->font_bouding_box[1], &font->font_bouding_box[2],
             &font->font_bouding_box[3]) < 4) {
    return PARSE_ERROR;
  }
  return SUCCESS;
}

ROLFRError parse_font_start_properties(struct ParserState *parser_state) {
  parser_state->section = PROPERTIES;
  return SUCCESS;
}

ROLFRError parse_font_end_properties(struct ParserState *parser_state) {
  parser_state->section = GLOBAL;
  return SUCCESS;
}

ROLFRError parse_font_chars(struct ParserState *parser_state, const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %zu", &font->n_glyphs) == 0) {
    return PARSE_ERROR;
  }
  font->glyphs = malloc(font->n_glyphs * sizeof(struct BDFGlyph));
  // Allocate enough space for all font bitmaps.
  // We take the bounding box of the font and compute the required memory for a single bitmap as follows:
  //
  // We need at least the height of the bounding box in bytes, multiplied
  // by the lowest number of bytes necessary to represent each row.
  // We compute the size necessary for a row by doing ((row - 1) >> 3) + 1.
  // With this formula, a line of 16 (requiring 2 bytes) would evaluate to
  //       ((16 - 1) >> 3 + 1) = 2,
  // while ((17 - 1) >> 3 + 1) = 3,
  // and   ((15 - 1) >> 3 + 1) = 2.
  // This is what we want.
  font->bitmaps = malloc(font->n_glyphs * font->font_bouding_box[1] *
                         (((font->font_bouding_box[0] - 1) >> 3) + 1));
  parser_state->bitmap_ptr = font->bitmaps;
  return SUCCESS;
}

ROLFRError parse_font_start_char(struct ParserState *parser_state,
                                 const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %s", font->glyphs[parser_state->current_glyph].name) ==
      0) {
    return PARSE_ERROR;
  }
  parser_state->section = GLYPH;
  return SUCCESS;
}

ROLFRError parse_font_end_char(struct ParserState *parser_state) {
  ++parser_state->current_glyph;
  parser_state->section = GLOBAL;
  return SUCCESS;
}

ROLFRError parse_font_char_encoding(struct ParserState *parser_state,
                                    const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %d",
             &font->glyphs[parser_state->current_glyph].encoding) == 0) {
    return PARSE_ERROR;
  }
  if (font->glyphs[parser_state->current_glyph].encoding == -1) {
    return UNSUPPORTED_ENCODING;
  }
  return SUCCESS;
}

ROLFRError parse_font_char_swidth(struct ParserState *parser_state,
                                  const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %f %f",
             &font->glyphs[parser_state->current_glyph].swidth[0],
             &font->glyphs[parser_state->current_glyph].swidth[1]) < 2) {
    return PARSE_ERROR;
  }
  return SUCCESS;
}

ROLFRError parse_font_char_dwidth(struct ParserState *parser_state,
                                  const char *line, struct BDFFont *font) {
  if (sscanf(line, "%*s %u %u",
             &font->glyphs[parser_state->current_glyph].dwidth[0],
             &font->glyphs[parser_state->current_glyph].dwidth[1]) < 2) {
    return PARSE_ERROR;
  }
  return SUCCESS;
}

ROLFRError parse_font_char_bounding_box(struct ParserState *parser_state,
                                        const char *line,
                                        struct BDFFont *font) {
  if (sscanf(line, "%*s %d %d %d %d",
             &font->glyphs[parser_state->current_glyph].bbx[0],
             &font->glyphs[parser_state->current_glyph].bbx[1],
             &font->glyphs[parser_state->current_glyph].bbx[2],
             &font->glyphs[parser_state->current_glyph].bbx[3]) < 4) {
    return PARSE_ERROR;
  }
  return SUCCESS;
}

ROLFRError parse_font_char_bitmap(struct ParserState *parser_state,
                                  const char *line, struct BDFFont *font) {
  parser_state->section = BITMAP;
  parser_state->current_bitmap_line = 0;
  font->glyphs[parser_state->current_glyph].bitmap = parser_state->bitmap_ptr;
  return SUCCESS;
}

ROLFRError parse_font_char_bitmap_line(struct ParserState *parser_state,
                                       const char *line, struct BDFFont *font) {
  size_t line_size = strlen(line) - 1;
  if (line_size % 2) {
    return PARSE_ERROR;
  }
  for (size_t i = 0; i < line_size; i += 2) {
    char byte_str[3] = {line[0], line[1], 0};
    unsigned char byte = strtoul(byte_str, NULL, 16);
    *(parser_state->bitmap_ptr) = byte;
    ++parser_state->bitmap_ptr;
    line += 2;
  }
  if (++parser_state->current_bitmap_line >= font->glyphs[parser_state->current_glyph].bbx[1]) {
    parser_state->section = GLYPH;
  }
  return SUCCESS;
}

ROLFRError parse_global_keyword(struct ParserState *parser_state, char *line,
                                const char *keyword, struct BDFFont *font) {
  switch (parser_state->section) {
  case GLOBAL:
    KEYWORD_CASE(keyword, "FONT", parse_font_name(line, font));
    KEYWORD_CASE(keyword, "SIZE", parse_font_size(line, font));
    KEYWORD_CASE(keyword, "FONTBOUNDINGBOX",
                 parse_font_bounding_box(line, font));
    KEYWORD_CASE(keyword, "STARTPROPERTIES",
                 parse_font_start_properties(parser_state));
    KEYWORD_CASE(keyword, "CHARS", parse_font_chars(parser_state, line, font));
    KEYWORD_CASE(keyword, "STARTCHAR",
                 parse_font_start_char(parser_state, line, font));
    FORWARD_ERR(ignore_keyword(line, font));
    break;
  case PROPERTIES:
    KEYWORD_CASE(keyword, "ENDPROPERTIES",
                 parse_font_end_properties(parser_state));
    FORWARD_ERR(ignore_keyword(line, font));
    break;
  case GLYPH:
    KEYWORD_CASE(keyword, "ENCODING",
                 parse_font_char_encoding(parser_state, line, font));
    KEYWORD_CASE(keyword, "SWIDTH",
                 parse_font_char_swidth(parser_state, line, font));
    KEYWORD_CASE(keyword, "DWIDTH",
                 parse_font_char_dwidth(parser_state, line, font));
    KEYWORD_CASE(keyword, "BBX",
                 parse_font_char_bounding_box(parser_state, line, font));
    KEYWORD_CASE(keyword, "BITMAP",
                 parse_font_char_bitmap(parser_state, line, font));
    KEYWORD_CASE(keyword, "ENDCHAR", parse_font_end_char(parser_state));
  case BITMAP:
    FORWARD_ERR(parse_font_char_bitmap_line(parser_state, line, font));
  }
  return SUCCESS;
}

ROLFRError parse_global_font(struct ParserState *parser_state, FILE *file,
                             struct BDFFont *font) {
  char *line = NULL;
  size_t line_size = 0;
  char *keyword = NULL;
  size_t keyword_size = 0;
  for (;;) {
    CHECK_GETLINE(line, line_size, file);
    // If we encounter ENDFONT, we parsed the whole file successfully
    if (!strncmp(line, "ENDFONT", sizeof("ENDFONT") - 1)) {
      break;
    }

    if (keyword_size < line_size) {
      keyword = realloc(keyword, line_size);
      keyword_size = line_size;
    }
    if (!sscanf(line, "%s", keyword)) {
      free(keyword);
      free(line);
      return PARSE_ERROR;
    }
    FORWARD_ERR(parse_global_keyword(parser_state, line, keyword, font));
  }
  if (line) {
    free(line);
  }
  if (keyword) {
    free(keyword);
  }
  return SUCCESS;
}

ROLFRError parse_bdf_font(FILE *file, struct BDFFont *font) {
  struct ParserState parser_state;
  parser_state.section = GLOBAL;
  parser_state.current_glyph = 0;
  char *line = NULL;
  size_t line_size;

  // We expect STARTFONT to be the first line
  CHECK_GETLINE(line, line_size, file);
  float ver;
  if (sscanf(line, "STARTFONT %f", &ver) == 0) {
    return PARSE_ERROR;
  }
  if (line) {
    free(line);
  }

  FORWARD_ERR(parse_global_font(&parser_state, file, font));
  return SUCCESS;
}

ROLFRError load_bdf_font_from_file(const char *path, ROLFont *result_font) {
  FILE *file = fopen(path, "r");
  if (!file) {
    return FILE_OPEN;
  }

  ROLFRError err;
  if ((err = parse_bdf_font(file, result_font)) != SUCCESS) {
    fclose(file);
    return err;
  } else {
    fclose(file);
    return SUCCESS;
  }
}

void free_bdf_font(struct BDFFont *font) {
  if (font->name)
    free(font->name);
  if (font->glyphs)
    free(font->glyphs);
  if (font->bitmaps) {
    free(font->bitmaps);
  }
  free(font);
}
