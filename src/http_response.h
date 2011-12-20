#ifndef _http_response_h
#define _http_response_h

typedef struct http_response {
  char* raw_response;
} http_response;

http_response* response;

void init_http_response();
char* serialize_http_response();
void free_http_response();

#endif
