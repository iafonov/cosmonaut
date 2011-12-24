#include <stdlib.h>

#include "http_request.h"
#include "string_util.h"
#include "log.h"
#include "configuration.h"

extern struct global_config* configuration;

char* extract_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

char* construct_url(char *path) {
  int len = strlen("http://") + strlen(configuration->server_name) + strlen(":") + strlen(configuration->server_port) + strlen(path);
  char* url = malloc_str(len);

  sprintf(url, "http://%s:%s%s", configuration->server_name, configuration->server_port, path);

  return url;
}

int request_url_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  char *path = extract_from_buffer(buf, len);

  request->raw_url = construct_url(path);
  request->url = parse_url(request->raw_url);

  free(path);
  return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len) {
  return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len) {
  return 0;
}

int body_cb(http_parser *p, const char *buf, size_t len) {
  return 0;
}

int count_body_cb(http_parser *p, const char *buf, size_t len) {
  return 0;
}

int message_begin_cb(http_parser *p) {
  return 0;
}

int headers_complete_cb(http_parser *p) {
  return 0;
}

int message_complete_cb (http_parser *p) {
  return 0;
}

static http_parser_settings settings = {
  .on_message_begin = message_begin_cb,
  .on_header_field = header_field_cb,
  .on_header_value = header_value_cb,
  .on_url = request_url_cb,
  .on_body = body_cb,
  .on_headers_complete = headers_complete_cb,
  .on_message_complete = message_complete_cb
};

http_request* http_request_init() {
  http_request* request = malloc(sizeof(http_request));
  request->parser = malloc(sizeof(http_parser));
  request->parser->data = request;
  http_parser_init(request->parser, HTTP_REQUEST);

  return request;
}

void http_request_free(http_request* request) {
  free(request->parser);
  free(request->raw_url);
  free_parsed_url(request->url);
  free(request);
}

void http_request_parse(http_request* request, char* raw_request_buf, int received) {
  int parsed = http_parser_execute(request->parser, &settings, raw_request_buf, received);
  if (parsed != received) {
    die("malformed http request");
  }

  info("method: %s", http_method_str(request->parser->method));
  info("full-url: [%s]", request->raw_url);
  info("request-path: [%s]", request->url->path);
}
