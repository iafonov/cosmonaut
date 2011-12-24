#ifndef _http_request_h
#define _http_request_h

#include "../deps/http_parser/http_parser.h"
#include "../deps/url_parser/url.h"

typedef struct http_request {
  parsed_url* url;
  char* raw_url;

  http_parser *parser;
} http_request;

http_request* http_request_init();
void http_request_free(http_request* request);
void http_request_parse(http_request* request, char* raw_request_buf, int received);

#endif
