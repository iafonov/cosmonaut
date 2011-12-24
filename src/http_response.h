#ifndef _http_response_h
#define _http_response_h

#include "headers_map.h"

typedef struct http_response {
  int code;
  char* header_summary;
  char* file_path; // relative to public_root
  char* content_type;
  int content_length;

  headers_map* http_headers;
  char* raw_response;
} http_response;

http_response* response;

void init_http_response();
void free_http_response();

char* serialize_headers(http_response* response);
char* serialize_http_response(http_response* response);

#endif
