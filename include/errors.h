#ifndef __ERRORS_H__
#define __ERRORS_H__

typedef enum ROLFRError {
  SUCCESS = 0,
  UNKNOWN_FORMAT,
  FILE_OPEN,
  FILE_READ,
  UNEXPECTED_EOF,
  PARSE_ERROR,
  UNSUPPORTED_ENCODING,
  UNDEFINED_GLYPH
} ROLFRError;

const char *get_rolfr_error_string(ROLFRError);

#endif // __ERRORS_H__
