#include <pcre.h>

#include "http_request.h"
#include "string_util.h"
#include "log.h"

int request_url_cb(http_parser *p, const char *buf, size_t len) {
  err("%s", buf);
  return 0;
}

int header_field_cb(http_parser *p, const char *buf, size_t len) {
  err("%s", buf);
  return 0;
}

int header_value_cb(http_parser *p, const char *buf, size_t len) {
  err("%s", buf);
  return 0;
}

int body_cb(http_parser *p, const char *buf, size_t len) {
  err("%s", buf);
  return 0;
}

int count_body_cb(http_parser *p, const char *buf, size_t len)
{
  err("%s", buf);
  return 0;
}

int message_begin_cb(http_parser *p) {
  err("mb");
  return 0;
}

int headers_complete_cb(http_parser *p) {
  err("mc");
  return 0;
}

int message_complete_cb (http_parser *p) {
  err("mc");
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

void init_http_request(HTTPRequest *request) {
  request->parser = malloc(sizeof(http_parser));
  http_parser_init(request->parser, HTTP_REQUEST);
}

void parse_http_request(HTTPRequest *request, char* raw_request_buf, int received) {
  int parsed = http_parser_execute(request->parser, &settings, raw_request_buf, received);
  if (parsed != received) {
    die("http response cannot be parsed");
  }
}
