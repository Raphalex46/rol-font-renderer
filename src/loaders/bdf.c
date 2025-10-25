#include "loaders/bdf.h"
#include "errors.h"

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

#define KEYWORD_CASE(line, keyword, font, token, parse_func)                   \
  do {                                                                         \
    if (!strcmp(keyword, token)) {                                             \
      FORWARD_ERR(remove_keyword(&line, token));                               \
      FORWARD_ERR(parse_func(line, font));                                     \
      return SUCCESS;                                                          \
    }                                                                          \
  } while (0)

struct ParserState {
  size_t current_line;
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
  if (sscanf(line, "%s", font_name) == 0) {
    free(font_name);
    return PARSE_ERROR;
  }
  font->name = font_name;
  return SUCCESS;
}

ROLFRError parse_global_keyword(char *line, const char *keyword,
                                struct BDFFont *font) {
  KEYWORD_CASE(line, keyword, font, "FONT", parse_font_name);
  FORWARD_ERR(ignore_keyword(line, font));
  return SUCCESS;
}

ROLFRError parse_global_font(FILE *file, struct BDFFont *font) {
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
    FORWARD_ERR(parse_global_keyword(line, keyword, font));
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

  FORWARD_ERR(parse_global_font(file, font));
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
  if (font->name) free(font->name);
  free(font);
}
