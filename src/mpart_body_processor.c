#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mpart_body_processor.h"
#include "attrs_map.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"

extern struct global_config* configuration;

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
  mpart_body_processor* processor = (mpart_body_processor*)p->data;

  processor->_last_header_name = copy_chunk_from_buffer(buf, len);

  return 0;
}

static int header_value_cb(multipart_parser* p, const char *buf, size_t len) {
  mpart_body_processor* processor = (mpart_body_processor*)p->data;

  char *header_value = copy_chunk_from_buffer(buf, len);
  headers_map_add(processor->part_headers, processor->_last_header_name, header_value);
  free(processor->_last_header_name);
  free(header_value);

  return 0;
}

static int headers_complete_cb(multipart_parser* p) {
  mpart_body_processor* processor = (mpart_body_processor*)p->data;
  http_request* request = (http_request*)processor->request;

  char* content_disposition = headers_map_get(processor->part_headers, "Content-Disposition");
  attrs_map* cd_attrs_map = attrs_map_init();

  if (str_starts_with(content_disposition, "form-data;")) {
    attrs_map_parse(cd_attrs_map, content_disposition + strlen("form-data;"));

    char* name = attrs_map_get(cd_attrs_map, "name");
    char* filename = attrs_map_get(cd_attrs_map, "filename");
    bool is_file = (filename != NULL);

    processor->_current_param = param_entry_reinit(processor->_current_param, name, NULL, is_file);

    if (is_file) {
      char *upload_folder_path = http_request_uploads_path(request);
      mkdir(upload_folder_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      char *file_path = malloc_str(strlen(upload_folder_path) + strlen("/") + strlen(filename));
      sprintf(file_path, "%s/%s", upload_folder_path, filename);
      processor->_current_param->file = fopen(file_path, "a");
    }
  }

  attrs_map_free(cd_attrs_map);
  return 0;
}

static int part_data_cb(multipart_parser* p, const char *buf, size_t len) {
  mpart_body_processor* processor = (mpart_body_processor*)p->data;
  char *data_chunk = copy_chunk_from_buffer(buf, len);

  param_entry_append(processor->_current_param, data_chunk);

  free(data_chunk);

  return 0;
}

static int part_data_begin_cb(multipart_parser* p) {
  mpart_body_processor* processor = (mpart_body_processor*)p->data;

  processor->part_headers = headers_map_init();
  return 0;
}

static int part_data_end_cb(multipart_parser* p) {
  mpart_body_processor* processor = (mpart_body_processor*)p->data;
  http_request* request = (http_request*)processor->request;

  params_map_add(request->params, processor->_current_param);

  param_entry_free(processor->_current_param);
  headers_map_free(processor->part_headers);
  return 0;
}

static int body_end_cb(multipart_parser* p) {
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

  processor->request = request;
  processor->parser = init_multipart_parser(boundary, &settings);
  processor->parser->data = processor;
  processor->_current_param = NULL;

  free(boundary);
  return processor;
}

void mpart_body_processor_free(mpart_body_processor* processor) {
  free_multipart_parser(processor->parser);
  free(processor);
}
