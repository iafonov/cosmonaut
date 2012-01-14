#include <stdio.h>
#include <stdlib.h>

#include "http_response.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"

http_response* http_response_init() {
  http_response* response = malloc(sizeof(http_response));

  response->header_summary = NULL;
  response->file_path = NULL;
  response->content_type = NULL;
  response->raw_response = NULL;

  response->headers = headers_map_init();

  return response;
}

void http_response_free(http_response* response) {
  free(response->file_path);
  free(response->content_type);
  free(response->raw_response);

  headers_map_free(response->headers);
  free(response);
}

char* build_header_header(http_response* response) {
  char* result = malloc_str(3 + strlen("HTTP/1.1 XXX ") + strlen(response->header_summary) + strlen("\n"));
  sprintf(result, "HTTP/1.1 %d %s\n", response->code, response->header_summary);

  return result;
}

char* http_response_serialize_headers(http_response* response) {
  char* http_header = build_header_header(response);
  char* content_length = create_str_from_int(response->content_length);

  headers_map_add(response->headers, "Content-Type", response->content_type);
  headers_map_add(response->headers, "Server", "Cosmonaut/0.0.1");
  headers_map_add(response->headers, "Content-Length", content_length);

  http_header = headers_map_serialize(response->headers, http_header);

  free(content_length);

  return http_header;
}
