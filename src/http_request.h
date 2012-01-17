#ifndef _http_request_h
#define _http_request_h

#define DATA_CHUNK_SIZE (1024)

#include <stdio.h>

#include "url.h"
#include "headers_map.h"
#include "params_map.h"

typedef struct http_request http_request;
typedef struct http_request_state http_request_state;

typedef void (*progress_hook)(http_request *request, int content_length, int parsed);

struct http_request {
  url* url;
  headers_map* headers;
  params_map* params;
  struct configuration* configuration;
  struct route* route;
  void *data;

  progress_hook progress_hook;

  char* uid;
  http_request_state* _s;
};

http_request* http_request_init();
char* http_request_uploads_path(http_request* request);
void http_request_free(http_request* request);
void http_request_parse(http_request* request, int socket_fd);

#endif
