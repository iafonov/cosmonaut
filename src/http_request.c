#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "http_request.h"
#include "string_util.h"
#include "log.h"
#include "configuration.h"
#include "attrs_map.h"
#include "multipart_parser.h"
#include "mpart_body_processor.h"

#define CR '\r'
#define LF '\n'

extern struct global_config* configuration;

// http parser callbacks
static int request_url_cb(http_parser *p, const char *buf, size_t len);
static int header_field_cb(http_parser *p, const char *buf, size_t len);
static int header_value_cb(http_parser *p, const char *buf, size_t len);
static int body_cb(http_parser *p, const char *buf, size_t len);
static int headers_complete_cb(http_parser *p);

static http_parser_settings settings = {
  .on_header_field = header_field_cb,
  .on_header_value = header_value_cb,
  .on_url = request_url_cb,
  .on_body = body_cb,
  .on_headers_complete = headers_complete_cb
};

static bool is_multipart(http_request *request) {
  char *content_type = headers_map_get(request->headers, "Content-Type");

  return content_type && str_starts_with(content_type, "multipart/form-data");
}

static char* extract_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

static char* construct_url(char *path) {
  int len = strlen("http://") + strlen(configuration->server_name) +
            strlen(":") + strlen(configuration->server_port) + strlen(path);

  char* url = malloc_str(len);
  sprintf(url, "http://%s:%s%s", configuration->server_name, configuration->server_port, path);
  return url;
}

static int request_url_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  char *path = extract_from_buffer(buf, len);

  request->raw_url = construct_url(path);
  request->url = parse_url(request->raw_url);

  free(path);
  return 0;
}

static int header_field_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;

  request->_last_header_name = extract_from_buffer(buf, len);
  return 0;
}

static int header_value_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;

  char *header_value = extract_from_buffer(buf, len);
  headers_map_add(request->headers, request->_last_header_name, header_value);
  free(request->_last_header_name);
  free(header_value);
  return 0;
}

static int headers_complete_cb(http_parser *p) {
  http_request* request = (http_request*)p->data;

  if (is_multipart(request)) {
    request->body_processor = mpart_body_processor_init(request);
    request->free_body_parser_func = (free_body_parser)mpart_body_processor_free;

    settings.on_body = mpart_body_process;
  }

  return 0;
}

static int body_cb(http_parser *p, const char *buf, size_t len) {
  info("generic dummy body parser");
  return 0;
}

http_request* http_request_init() {
  http_request* request = malloc(sizeof(http_request));
  request->parser = malloc(sizeof(http_parser));
  request->parser->data = request;
  http_parser_init(request->parser, HTTP_REQUEST);

  request->headers = headers_map_init();

  return request;
}

void http_request_free(http_request* request) {
  request->free_body_parser_func(request->body_processor);

  free(request->parser);
  free(request->raw_url);
  free_parsed_url(request->url);
  headers_map_free(request->headers);
  free(request);
}

void http_request_parse(http_request* request, int socket_fd) {
  char request_buffer[DATA_CHUNK_SIZE];
  int received = 0;

  while ((received = recv(socket_fd, &request_buffer, DATA_CHUNK_SIZE, 0))) {
    http_parser_execute(request->parser, &settings, request_buffer, received);
    if (received < DATA_CHUNK_SIZE) break;
  }
}
