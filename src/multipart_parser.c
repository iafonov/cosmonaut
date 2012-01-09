#include <stdlib.h>
#include <string.h>

#include "multipart_parser.h"
#include "log.h"

#ifdef DEBUG
#include <stdio.h>
#define log(M, ...) fprintf(stderr, "[HTTP_MULTIPART_PARSER] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define log(M, ...)
#endif

#define NOTIFY_CB(FOR)                                               \
do {                                                                 \
  if (p->settings->on_##FOR) {                                       \
    if (p->settings->on_##FOR(p) != 0) {                             \
      return i;                                                      \
    }                                                                \
  }                                                                  \
} while (0)

#define EMIT_DATA_CB(FOR)                                            \
do {                                                                 \
  if (p->settings->on_##FOR) {                                       \
    if (p->settings->on_##FOR(p, (buf + mark), (i - mark)) != 0) {   \
      return i;                                                      \
    }                                                                \
  }                                                                  \
} while (0)

#define EMIT_PART_DATA_CB(FOR)                                       \
do {                                                                 \
  if (p->settings->on_##FOR) {                                       \
    p->_parsed = p->_parsed + (i - mark);                            \
    if (p->settings->on_##FOR(p, (buf + mark), (i - mark)) != 0) {   \
      return i;                                                      \
    }                                                                \
  }                                                                  \
} while (0)


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
multipart_parser* init_multipart_parser(char *boundary, multipart_parser_settings* settings) {
  multipart_parser* p = malloc(sizeof(multipart_parser));
  p->index = 0;
  p->state = s_start;
  p->settings = settings;

  p->_multipart_boundary = strdup(boundary);
  p->_boundary_length = strlen(boundary);
  p->_flags = 0;
  p->_lookbehind = NULL;
  p->_parsed = 0;
  return p;
}

void free_multipart_parser(multipart_parser* p) {
  free(p->_multipart_boundary);
  free(p->_lookbehind);
  free(p);
}

int multipart_parser_execute(multipart_parser* p, const char *buf, size_t len) {
  int i = 0;
  int mark = 0;
  int prevIndex = 0;

  for (i = 0; i < len; i++) {
    char c = buf[i];
    switch (p->state) {
      case s_start:
        log("s_start");
        p->_lookbehind = malloc(p->_boundary_length + 8 + 1);

        p->index = 0;
        p->state = s_start_boundary;
        break;
      case s_start_boundary:
        log("s_start_boundary");
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
          NOTIFY_CB(part_data_begin);
          p->state = s_header_field_start;
          break;
        }

        if (c != p->_multipart_boundary[p->index + 1]) {
          return i;
        }

        p->index++;
        break;
      case s_header_field_start:
        log("s_header_field_start");
        mark = i;
        p->state = s_header_field;
      case s_header_field:
        log("s_header_field");
        if (c == CR) {
          p->state = s_headers_almost_done;
          break;
        }

        if (c == HYPHEN) {
          break;
        }

        if (c == COLON) {
          EMIT_DATA_CB(header_field);
          p->state = s_header_value_start;
          break;
        }

        char cl = LOWER(c);
        if (cl < A || cl > Z) {
          return i;
        }

        if (i == len - 1) EMIT_DATA_CB(header_field);
        break;
      case s_headers_almost_done:
        log("s_headers_almost_done");
        if (c != LF) {
          return i;
        }

        p->state = s_part_data_start;
        break;
      case s_header_value_start:
        log("s_header_value_start");
        if (c == SPACE) {
          break;
        }

        mark = i;
        p->state = s_header_value;
      case s_header_value:
        log("s_header_value");
        if (c == CR) {
          EMIT_DATA_CB(header_value);
          p->state = s_header_value_almost_done;
        }

        if (i == len - 1) EMIT_DATA_CB(header_value);
        break;
      case s_header_value_almost_done:
        log("s_header_value_almost_done");
        if (c != LF) {
          return i;
        }
        p->state = s_header_field_start;
        break;
      case s_part_data_start:
        log("s_part_data_start");
        NOTIFY_CB(headers_complete);
        mark = i;
        p->state = s_part_data;
      case s_part_data:
        log("s_part_data");
        prevIndex = p->index;

        if (p->index < p->_boundary_length) {
          if (p->_multipart_boundary[p->index] == c) {
            if (p->index == 0) {
              // very ugly way to omit emitting trailing CR+LF which doesn't belong to file but
              // rather belong to multipart stadard
              int adjustment = 0;
              if (buf[i - 1] == LF && buf[i - 2] == CR && buf[i + 1] == '-') {
                adjustment = 2;
              }

              i = i - adjustment;
              EMIT_PART_DATA_CB(part_data);
              i = i + adjustment;
            }
            p->index++;
          } else {
            p->index = 0;
          }
        } else if (p->index == p->_boundary_length) {
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
        } else if (p->index - 1 == p->_boundary_length)  {
          if (p->_flags & f_part_boundary) {
            p->index = 0;
            if (c == LF) {
              // unset the PART_BOUNDARY flag
              p->_flags &= ~f_part_boundary;
              NOTIFY_CB(part_data_end);
              NOTIFY_CB(part_data_begin);
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
        } else if (p->index - 2 == p->_boundary_length)  {
          if (c == CR) {
            p->index++;
          } else {
            p->index = 0;
          }
        } else if (p->index - p->_boundary_length == 3)  {
          p->index = 0;
          if (c == LF) {
            NOTIFY_CB(part_data_end);
            NOTIFY_CB(body_end);
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
          p->_parsed = p->_parsed + prevIndex;
          p->settings->on_part_data(p, p->_lookbehind, prevIndex);
          prevIndex = 0;
          mark = i;
        }

        if (p->index == 0 && i == len - 1) {
          i++;
          EMIT_PART_DATA_CB(part_data);
        }
        break;
      case s_end:
        log("s_end");
        break;
      default:
        log("Multipart parser unrecoverable error");
        return 0;
    }
  }

  return i;
}