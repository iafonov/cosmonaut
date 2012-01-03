#ifndef _multipart_parser_h
#define _multipart_parser_h

typedef struct multipart_parser {
  void* data;
  int index;
  unsigned char state;

  int _flags;
  char* _lookbehind;
  char* _multipart_boundary;
} multipart_parser;

typedef struct multipart_parser_settings multipart_parser_settings;

typedef int (*multipart_data_cb) (multipart_parser*, const char *at, size_t length);
typedef int (*multipart_notify_cb) (multipart_parser*);

struct multipart_parser_settings {
  multipart_data_cb header_field;
  multipart_data_cb header_value;
  multipart_data_cb part_data;
  multipart_notify_cb part_data_begin;
  multipart_notify_cb part_data_end;
  multipart_notify_cb body_end;
};

multipart_parser* init_multipart_parser(char *boundary);
void free_multipart_parser(multipart_parser* p);

int multipart_parser_execute(multipart_parser* p, const char *buf, size_t len);

#endif