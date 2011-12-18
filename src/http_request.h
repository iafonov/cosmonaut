#ifndef _http_request_h
#define _http_request_h

#include "../deps/http_parser/http_parser.h"

typedef struct HTTPRequest {
  char* path;

  http_parser *parser;
} HTTPRequest;

HTTPRequest request;

void init_http_request();
void parse_http_request(char* raw_request_buf, int received);

#endif
