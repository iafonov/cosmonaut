#ifndef _http_request_h
#define _http_request_h

#include "../deps/http_parser/http_parser.h"
#include "../deps/url_parser/url.h"

typedef struct http_request {
  parsed_url* url;

  http_parser *parser;
} http_request;

http_request* request;

void init_http_request();
void free_http_request();
void parse_http_request(char* raw_request_buf, int received);

#endif
