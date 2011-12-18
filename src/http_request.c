#include <stdlib.h>

#include "http_request.h"
#include "string_util.h"
#include "log.h"

char* extract(const char *buf, size_t len) {
  char *field = malloc(len + 1);
  strncat(field, buf + 1, len);
  field[len + 1] = '\0';

  return field;
}

int request_url_cb(http_parser *p, const char *buf, size_t len) {
  request.path = extract(buf, len);
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

void init_http_request() {
  request.parser = malloc(sizeof(http_parser));
  http_parser_init(request.parser, HTTP_REQUEST);
}

void parse_http_request(char* raw_request_buf, int received) {
  debug("parsing start");
  int parsed = http_parser_execute(request.parser, &settings, raw_request_buf, received);
  debug("parsing complete. parsed:%d", parsed);
  if (parsed != received) {
    die("http response cannot be parsed");
  }
}
