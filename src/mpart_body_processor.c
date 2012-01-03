#include <stdlib.h>
#include <string.h>

#include "mpart_body_processor.h"
#include "attrs_map.h"
#include "string_util.h"
#include "log.h"

static int header_field_cb(multipart_parser* p, const char *buf, size_t len);
static int header_value_cb(multipart_parser* p, const char *buf, size_t len);
static int part_data_cb(multipart_parser* p, const char *buf, size_t len);
static int part_data_begin_cb(multipart_parser* p);
static int part_data_end_cb(multipart_parser* p);
static int body_end_cb(multipart_parser* p);

static multipart_parser_settings settings = {
  .on_header_field = header_field_cb,
  .on_header_value = header_value_cb,
  .on_part_data = part_data_cb,
  .on_part_data_begin = part_data_begin_cb,
  .on_part_data_end = part_data_end_cb,
  .on_body_end = body_end_cb
};

static char* copy_chunk_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

static int header_field_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  info("header_field_cb = [%s]", value);
  free(value);

  return 0;
}

static int header_value_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  info("header_value_cb = [%s]", value);
  free(value);

  return 0;
}

static int part_data_cb(multipart_parser* p, const char *buf, size_t len) {
  char* value = copy_chunk_from_buffer(buf, len);
  info("part_data_cb = [%s]", value);
  free(value);

  return 0;
}

static int part_data_begin_cb(multipart_parser* p) {
  info("--part_data_begin_cb");
  return 0;
}

static int part_data_end_cb(multipart_parser* p) {
  info("--part_data_end_cb");
  return 0;
}

static int body_end_cb(multipart_parser* p) {
  info("--body_end_cb");
  return 0;
}

static char* get_boundary(http_request *request) {
  char *content_type = headers_map_get(request->headers, "Content-Type");
  char *boundary = NULL;

  attrs_map *map = attrs_map_init();

  attrs_map_parse(map, content_type + strlen("multipart/form-data;"));
  boundary = strdup(strcat(strdup("--"), attrs_map_get(map, "boundary")));

  attrs_map_free(map);
  return boundary;
}

// public api
mpart_body_processor* mpart_body_processor_init(http_request* request) {
  mpart_body_processor* processor = malloc(sizeof(mpart_body_processor));

  processor->parser = init_multipart_parser(get_boundary(request), &settings);
  processor->parser->data = request;

  return processor;
}

void mpart_body_processor_free(mpart_body_processor* processor) {
  free_multipart_parser(processor->parser);
  free(processor);
}

int mpart_body_process(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  return multipart_parser_execute(processor->parser, buf, len);
}
