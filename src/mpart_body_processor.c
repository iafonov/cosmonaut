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
static int headers_complete_cb(multipart_parser* p);
static int part_data_end_cb(multipart_parser* p);
static int body_end_cb(multipart_parser* p);

static multipart_parser_settings settings = {
  .on_header_field = header_field_cb,
  .on_header_value = header_value_cb,
  .on_part_data = part_data_cb,
  .on_part_data_begin = part_data_begin_cb,
  .on_headers_complete = headers_complete_cb,
  .on_part_data_end = part_data_end_cb,
  .on_body_end = body_end_cb
};

static char* copy_chunk_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

static int header_field_cb(multipart_parser* p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  processor->_last_header_name = copy_chunk_from_buffer(buf, len);

  return 0;
}

static int header_value_cb(multipart_parser* p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  char *header_value = copy_chunk_from_buffer(buf, len);
  headers_map_add(processor->part_headers, processor->_last_header_name, header_value);
  free(processor->_last_header_name);
  free(header_value);

  return 0;
}

static int headers_complete_cb(multipart_parser* p) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  char* content_disposition = headers_map_get(processor->part_headers, "Content-Disposition");
  attrs_map* cd_attrs_map = attrs_map_init();

  if (str_starts_with(content_disposition, "form-data;")) {
    attrs_map_parse(cd_attrs_map, content_disposition + strlen("form-data;"));

    char* name = attrs_map_get(cd_attrs_map, "name");
    char* filename = attrs_map_get(cd_attrs_map, "filename");

    processor->_current_param = param_entry_reinit(processor->_current_param, name, NULL, (filename != NULL));
  }

  attrs_map_free(cd_attrs_map);
  return 0;
}

static int part_data_cb(multipart_parser* p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  processor->_current_param->val = str_concat(processor->_current_param->val, copy_chunk_from_buffer(buf, len));

  return 0;
}

static int part_data_begin_cb(multipart_parser* p) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  info("--part_data_begin_cb");
  processor->part_headers = headers_map_init();
  return 0;
}

static int part_data_end_cb(multipart_parser* p) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  params_map_add(request->params, processor->_current_param);

  param_entry_free(processor->_current_param);
  headers_map_free(processor->part_headers);
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

  boundary = str_concat("--", attrs_map_get(map, "boundary"));

  attrs_map_free(map);
  return boundary;
}

// public api
mpart_body_processor* mpart_body_processor_init(http_request* request) {
  mpart_body_processor* processor = malloc(sizeof(mpart_body_processor));
  char *boundary = get_boundary(request);

  processor->parser = init_multipart_parser(boundary, &settings);
  processor->parser->data = request;
  processor->_current_param = NULL;

  free(boundary);
  return processor;
}

void mpart_body_processor_free(mpart_body_processor* processor) {
  free_multipart_parser(processor->parser);
  free(processor);
}

int mpart_body_process(http_parser *p, const char *buf, size_t len) {
  http_request* request = (http_request*)p->data;
  mpart_body_processor* processor = (mpart_body_processor*)request->body_processor;

  multipart_parser_execute(processor->parser, buf, len);
  return 0;
}
