#ifndef _http_request_h
#define _http_request_h

#define DATA_CHUNK_SIZE (1024)

#include <stdio.h>

#include "../deps/http_parser/http_parser.h"
#include "../deps/url_parser/url.h"
#include "headers_map.h"
#include "multipart_parser.h"

typedef struct http_request http_request;

struct http_request {
  parsed_url* url;
  char* raw_url;
  headers_map* headers;

  http_parser *parser;
  multipart_parser *body_parser;

  // state vairables used during parsing
  char* _last_header_name;
};

http_request* http_request_init();
void http_request_free(http_request* request);
void http_request_parse(http_request* request, int socket_fd);

#endif
