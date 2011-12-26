#ifndef _http_response_h
#define _http_response_h

#include "headers_map.h"

typedef struct http_response {
  int code;
  char* header_summary;
  char* file_path; // relative to public_root
  char* content_type;
  int content_length;

  headers_map* headers;
  char* raw_response;
} http_response;

http_response* http_response_init();
void http_response_free(http_response* response);

char* http_response_serialize_headers(http_response* response);

#endif
