#include <stdlib.h>
#include <string.h>

#include "mpart_body_processor.h"
#include "attrs_map.h"
#include "log.h"

static char* get_boundary(http_request *request) {
  char *content_type = headers_map_get(request->headers, "Content-Type");
  char *boundary = NULL;

  attrs_map *map = attrs_map_init();

  attrs_map_parse(map, content_type + strlen("multipart/form-data;"));
  boundary = strdup(strcat(strdup("--"), attrs_map_get(map, "boundary")));

  attrs_map_free(map);
  return boundary;
}

mpart_body_processor* mpart_body_processor_init(http_request* request) {
  mpart_body_processor* processor = malloc(sizeof(mpart_body_processor));

  processor->parser = init_multipart_parser(get_boundary(request));
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
