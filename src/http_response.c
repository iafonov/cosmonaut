#include <stdio.h>
#include <stdlib.h>

#include "http_response.h"
#include "string_util.h"
#include "log.h"
#include "../deps/concat/concat.h"

#define MAX_HEADERS_COUNT 25

void init_http_response() {
  response = malloc(sizeof(http_response));
  response->headers = sm_new(MAX_HEADERS_COUNT);
}

void free_http_response() {
  sm_delete(response->headers);
  free(response);
}

char* build_header_header(http_response* response) {
  char* result = malloc_str(concat_len("HTTP/1.1", " ", malloc(response->code / 100), " ", response->header_summary, NULL));
  sprintf(result, "HTTP/1.1 %d %s", response->code, response->header_summary);
  return result;
}

void reduce_headers(const char *name, const char *value, const void *acc) {
  const char *http_headers = (char *)acc;
  int len = concat_len(name, ": ", value, "\n", NULL);

  err("%s: %s", name, value);
  err("acc: %s", http_headers);
}

char* serialize_headers(http_response* response) {
  char* http_header = build_header_header(response);

  sm_put(response->headers, "Content-Type", response->content_type);
  sm_put(response->headers, "Server", "Cosmonaut/0.0.1");
  sm_put(response->headers, "Content-Length", "31337");

  sm_enum(response->headers, reduce_headers, http_header);

  return http_header;
}

char* serialize_http_response(http_response* response) {
  char* buffer = "HTTP/1.1 405 Method Not Allowed\n"
                 "Content-Type: text/plain;\n"
                 "Content-Length:81\n"
                 "Server: Cosmonaut/0.0.1\n"
                 "\n"
                 "405 Method not allowed.1Supported methods are GET, POST and HEAD.\n"
                 "Cosmonaut/0.0.1\n";

  return buffer;
}
