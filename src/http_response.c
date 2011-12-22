#include <stdio.h>
#include <stdlib.h>

#include "http_response.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"
#include "../deps/concat/concat.h"

extern struct global_config* configuration;

#define MAX_HEADERS_COUNT 25

void init_http_response() {
  response = malloc(sizeof(http_response));

  response->header_summary = NULL;
  response->file_path = NULL;
  response->content_type = NULL;
  response->raw_response = NULL;

  response->headers = sm_new(MAX_HEADERS_COUNT);
}

void free_http_response() {
  free(response->file_path);

  sm_delete(response->headers);
  free(response);
}

char* build_header_header(http_response* response) {
  char* result = malloc_str(3 + concat_len("HTTP/1.1 XXX ", response->header_summary, "\n", NULL));
  sprintf(result, "HTTP/1.1 %d %s\n", response->code, response->header_summary);

  return result;
}

char* reduce_headers(const char *name, const char *value) {
  int len = concat_len(name, ": ", value, "\n", NULL);
  char* result = malloc_str(len);

  sprintf(result, "%s: %s\n", name, value);

  return result;
}

char* serialize_headers(http_response* response) {
  char* http_header = build_header_header(response);
  char* content_length = create_str_from_int(response->content_length);

  sm_put(response->headers, "Content-Type", response->content_type);
  sm_put(response->headers, "Server", "Cosmonaut/0.0.1");
  sm_put(response->headers, "Content-Length", content_length);

  char* headers = sm_reduce(response->headers, reduce_headers);

  http_header = realloc(http_header, strlen(http_header) + strlen(headers) + 1);
  char* result = concat(http_header, headers, "\n", NULL);

  free(headers);
  free(http_header);
  free(content_length);

  return result;
}
