#ifndef _http_response_h
#define _http_response_h

#include "../deps/strmap/strmap.h"

typedef struct http_response {
  int code;
  char* header_summary;
  char* file_path; // relative to public_root
  char* content_type;
  int content_length;

  StrMap *headers;
  char* raw_response;
} http_response;

http_response* response;

void init_http_response();
char* serialize_headers(http_response* response);
char* serialize_http_response(http_response* response);
void free_http_response();

#endif
