#ifndef _multipart_parser_h
#define _multipart_parser_h

#include <sys/types.h>

typedef struct multipart_parser_settings multipart_parser_settings;
typedef struct multipart_parser multipart_parser;

struct multipart_parser {
  void* data;
  int index;
  unsigned char state;
  multipart_parser_settings* settings;

  int _boundary_length;
  int _flags;
  char* _lookbehind;
  char* _multipart_boundary;
};

typedef int (*multipart_data_cb) (multipart_parser*, const char *at, size_t length);
typedef int (*multipart_notify_cb) (multipart_parser*);

struct multipart_parser_settings {
  multipart_data_cb on_header_field;
  multipart_data_cb on_header_value;
  multipart_data_cb on_part_data;

  multipart_notify_cb on_part_data_begin;
  multipart_notify_cb on_part_data_end;
  multipart_notify_cb on_body_end;
};

multipart_parser* init_multipart_parser(char *boundary, multipart_parser_settings* settings);
void free_multipart_parser(multipart_parser* p);

int multipart_parser_execute(multipart_parser* p, const char *buf, size_t len);

#endif