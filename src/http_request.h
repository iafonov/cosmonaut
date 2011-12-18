#ifndef _http_request_h
#define _http_request_h

#include "../deps/http_parser/http_parser.h"

typedef enum { GET, POST, HEAD, UNSUPPORTED } HttpMethod;

typedef struct HTTPRequest {
  int method;
  char* path;
  int valid;

  http_parser *parser;
} HTTPRequest;

void init_http_request(HTTPRequest *request);
void parse_http_request(HTTPRequest *request, char* raw_request_buf, int received);

#endif
