#include <stdio.h>
#include <stdlib.h>

#include "http_response.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"
#include "../deps/concat/concat.h"

extern struct global_config* configuration;

void init_http_response() {
  response = malloc(sizeof(http_response));

  response->header_summary = NULL;
  response->file_path = NULL;
  response->content_type = NULL;
  response->raw_response = NULL;

  response->http_headers = headers_map_init();
}

void free_http_response() {
  free(response->file_path);

  headers_map_free(response->http_headers);
  free(response);
}

char* build_header_header(http_response* response) {
  char* result = malloc_str(3 + concat_len("HTTP/1.1 XXX ", response->header_summary, "\n", NULL));
  sprintf(result, "HTTP/1.1 %d %s\n", response->code, response->header_summary);

  return result;
}

char* serialize_headers(http_response* response) {
  char* http_header = build_header_header(response);
  char* content_length = create_str_from_int(response->content_length);

  headers_map_add(response->http_headers, "Content-Type", response->content_type);
  headers_map_add(response->http_headers, "Server", "Cosmonaut/0.0.1");
  headers_map_add(response->http_headers, "Content-Length", content_length);

  http_header = headers_map_serialize(response->http_headers, http_header);

  free(content_length);

  return http_header;
}
