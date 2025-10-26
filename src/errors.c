#include "errors.h"

const char *get_rolfr_error_string(ROLFRError err) {
  switch (err) {
    case SUCCESS:
      return "success";
    case UNKNOWN_FORMAT:
      return "unknown format";
    case FILE_OPEN:
      return "failed to open file";
    case FILE_READ:
      return "failed to read file";
    case UNEXPECTED_EOF:
      return "unexpected end of file";
    case PARSE_ERROR:
      return "parse error";
    default:
      return "unkown error";
  }
}
