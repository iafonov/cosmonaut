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

#include "../deps/http_parser/http_parser.h"

#define CR '\r'
#define LF '\n'

typedef void (*free_body_parser) (void*);

struct http_request_state {
  void* body_processor;
  free_body_parser free_body_parser_func;
  char* _last_header_name;
};

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
  return str_starts_with(headers_map_get(request->headers, "Content-Type"), "multipart/form-data");
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
  char *request_url = construct_url(path);

  request->url = url_init(request_url);

  free(path);
  free(request_url);
  return 0;
}

static int header_field_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;

  request->_s->_last_header_name = extract_from_buffer(buf, len);
  return 0;
}

static int header_value_cb(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;

  char *header_value = extract_from_buffer(buf, len);
  headers_map_add(request->headers, request->_s->_last_header_name, header_value);
  free(request->_s->_last_header_name);
  free(header_value);
  return 0;
}

static int mpart_body_process(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->_s->body_processor;

  multipart_parser_execute(processor->parser, buf, len);
  return 0;
}

static int headers_complete_cb(http_parser *p) {
  http_request* request = (http_request*)p->data;

  if (is_multipart(request)) {
    request->_s->body_processor = mpart_body_processor_init(request);
    request->_s->free_body_parser_func = (free_body_parser)mpart_body_processor_free;

    settings.on_body = mpart_body_process;
  }

  return 0;
}

static int body_cb(http_parser *p, const char *buf, size_t len) {
  // generic body parser prototype
  return 0;
}

http_request* http_request_init() {
  http_request* request = malloc(sizeof(http_request));

  request->headers = headers_map_init();
  request->params = params_map_init();
  request->uid = str_random(20);
  request->configuration = configuration;

  request->_s = malloc(sizeof(http_request_state));
  request->_s->free_body_parser_func = NULL;

  return request;
}

void http_request_free(http_request* request) {
  if (request->_s->free_body_parser_func != NULL) {
    request->_s->free_body_parser_func(request->_s->body_processor);
  }

  url_free(request->url);
  headers_map_free(request->headers);
  params_map_free(request->params);
  free(request->uid);
  free(request->_s);
  free(request);
}

char* http_request_uploads_path(http_request* request) {
  char* result = malloc_str(strlen(configuration->uploads_root) + strlen("/") + strlen(request->uid));
  sprintf(result, "%s/%s", configuration->uploads_root, request->uid);

  return result;
}

void http_request_parse(http_request* request, int socket_fd) {
  char request_buffer[DATA_CHUNK_SIZE];
  int received = 0;

  http_parser* parser = malloc(sizeof(http_parser));
  http_parser_init(parser, HTTP_REQUEST);
  parser->data = request;

  while ((received = recv(socket_fd, &request_buffer, DATA_CHUNK_SIZE, 0))) {
    http_parser_execute(parser, &settings, request_buffer, received);
    if (received < DATA_CHUNK_SIZE) break;
  }

  free(parser);
}
