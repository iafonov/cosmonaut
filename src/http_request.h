#ifndef _http_request_h
#define _http_request_h

#define DATA_CHUNK_SIZE (1024)

#include <stdio.h>

#include "../deps/http_parser/http_parser.h"
#include "../deps/url_parser/url.h"
#include "headers_map.h"

typedef struct http_request http_request;

typedef void (*free_body_parser) (void*);

struct http_request {
  parsed_url* url;
  char* raw_url;
  headers_map* headers;

  http_parser *parser;

  void* body_processor;
  free_body_parser free_body_parser_func;

  // state vairable used during parsing
  char* _last_header_name;
};

http_request* http_request_init();
void http_request_free(http_request* request);
void http_request_parse(http_request* request, int socket_fd);

#endif
