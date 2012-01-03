#include <stdlib.h>
#include <string.h>

#include "multipart_parser.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define log(M, ...) fprintf(stderr, "[HTTP_MULTIPART_PARSER] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define log(M, ...)
#endif

#define LF 10
#define CR 13
#define SPACE 32
#define HYPHEN 45
#define COLON 58
#define A 97
#define Z 122
#define LOWER(c) (unsigned char)(c | 0x20)

enum state {
  s_uninitialized = 1,
  s_start,
  s_start_boundary,
  s_header_field_start,
  s_header_field,
  s_headers_almost_done,
  s_header_value_start,
  s_header_value,
  s_header_value_almost_done,
  s_part_data_start,
  s_part_data,
  s_end
};

enum parser_flags {
  f_part_boundary = 1,
  f_last_boundary
};

// public api
multipart_parser* init_multipart_parser(char *boundary) {
  multipart_parser* p = malloc(sizeof(multipart_parser));
  p->index = 0;
  p->state = s_start;
  p->_multipart_boundary = strdup(boundary);
  p->_flags = 0;
  p->_lookbehind = NULL;
  return p;
}

#include "string_util.h"
char* copy_chunk_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

void free_multipart_parser(multipart_parser* p) {
  free(p->_multipart_boundary);
  free(p->_lookbehind);
  free(p);
}

static void header_field_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  log("header_field_cb = [%s]", value);
  free(value);
}

static void header_value_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  log("header_value_cb = [%s]", value);
  free(value);
}

static void part_data_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  log("part_data_cb = [%s]", value);
  free(value);
}

static void part_data_begin_cb(multipart_parser* p) {
  log("--part_data_begin_cb");
}

static void part_data_end_cb(multipart_parser* p) {
  log("--part_data_end_cb");
}

static void body_end_cb(multipart_parser* p) {
  log("--body_end_cb");
}

int multipart_parser_execute(multipart_parser* p, const char *buf, size_t len) {
  log("PARSING %s", buf);
  int i = 0;
  int mark = 0;
  int prevIndex = 0;
  int boundaryLength = 0;

  for (i = 0; i < len; i++) {
    char c = buf[i];
    switch (p->state) {
      case s_start:
        // log("s_start");
        boundaryLength = strlen(p->_multipart_boundary);
        log("boundaryLength set to %d", boundaryLength);
        p->_lookbehind = malloc(boundaryLength + 8 + 1);

        p->index = 0;
        p->state = s_start_boundary;
        break;
      case s_start_boundary:
        if (p->index == strlen(p->_multipart_boundary) - 1) {
          if (c != CR) {
            return i;
          }
          p->index++;
          break;
        } else if (p->index == (strlen(p->_multipart_boundary))) {
          if (c != LF) {
            return i;
          }
          p->index = 0;
          p->state = s_header_field_start;
          break;
        }

        if (c != p->_multipart_boundary[p->index + 1]) {
          return i;
        }

        p->index++;
        break;
      case s_header_field_start:
        // log("s_header_field_start");
        mark = i;
        p->state = s_header_field;
      case s_header_field:
        // log("s_header_field");
        if (c == CR) {
          p->state = s_headers_almost_done;
          break;
        }

        if (c == HYPHEN) {
          break;
        }

        if (c == COLON) {
          header_field_cb(p, (buf + mark), (i - mark));
          p->state = s_header_value_start;
          break;
        }

        char cl = LOWER(c);
        if (cl < A || cl > Z) {
          return i;
        }
        break;
      case s_headers_almost_done:
        // log("s_headers_almost_done");
        if (c != LF) {
          return i;
        }

        p->state = s_part_data_start;
        break;
      case s_header_value_start:
        // log("s_header_value_start");
        if (c == SPACE) {
          break;
        }

        mark = i;
        p->state = s_header_value;
      case s_header_value:
        // log("s_header_value");
        if (c == CR) {
          header_value_cb(p, (buf + mark), (i - mark));
          p->state = s_header_value_almost_done;
        }
        break;
      case s_header_value_almost_done:
        // log("s_header_value_almost_done");
        if (c != LF) {
          return i;
        }
        p->state = s_header_field_start;
        break;
      case s_part_data_start:
        // log("s_part_data_start");
        mark = i;
        p->state = s_part_data;
      case s_part_data:
        prevIndex = p->index;

        if (p->index == 0) {
          while (i + boundaryLength <= len) {
            if (strchr(p->_multipart_boundary, buf[i + boundaryLength - 1]) != NULL) {
              break;
            }

            i += boundaryLength;
          }
          c = buf[i];
        }

        if (p->index < boundaryLength) {
          if (p->_multipart_boundary[p->index] == c) {
            if (p->index == 0) {
              part_data_cb(p, (buf + mark), (i - mark));
            }
            p->index++;
          } else {
            p->index = 0;
          }
        } else if (p->index == boundaryLength) {
          p->index++;
          if (c == CR) {
            // CR = part boundary
            p->_flags |= f_part_boundary;
          } else if (c == HYPHEN) {
            // HYPHEN = end boundary
            p->_flags |= f_last_boundary;
          } else {
            p->index = 0;
          }
        } else if (p->index - 1 == boundaryLength)  {
          if (p->_flags & f_part_boundary) {
            p->index = 0;
            if (c == LF) {
              // unset the PART_BOUNDARY flag
              p->_flags &= ~f_part_boundary;
              part_data_end_cb(p);
              part_data_begin_cb(p);
              p->state = s_header_field_start;
              break;
            }
          } else if (p->_flags & f_last_boundary) {
            if (c == HYPHEN) {
              p->index++;
            } else {
              p->index = 0;
            }
          } else {
            p->index = 0;
          }
        } else if (p->index - 2 == boundaryLength)  {
          if (c == CR) {
            p->index++;
          } else {
            p->index = 0;
          }
        } else if (p->index - boundaryLength == 3)  {
          p->index = 0;
          if (c == LF) {
            part_data_end_cb(p);
            body_end_cb(p);
            p->state = s_end;
            break;
          }
        }

        if (p->index > 0) {
          // when matching a possible boundary, keep a lookbehind reference
          // in case it turns out to be a false lead
          p->_lookbehind[p->index - 1] = c;
        } else if (prevIndex > 0) {
          // if our boundary turned out to be rubbish, the captured lookbehind
          // belongs to partData
          part_data_cb(p, p->_lookbehind, prevIndex);
          prevIndex = 0;
          mark = i;
        }
        break;
      case s_end:
        log("s_end");
        break;
      default:
        log("Multipart parser unrecoverable error");
        return 1;
    }
  }

  return 0;
}