#ifndef _mpart_body_processor_h
#define _mpart_body_processor_h

#include "multipart_parser.h"
#include "http_request.h"
#include "params_map.h"

typedef struct mpart_body_processor {
  http_request* request;
  multipart_parser* parser;

  // headers of current part - don't try to use it outside callbacks it's reset on every part
  headers_map* part_headers;
  char* _last_header_name;
  param_entry* _current_param;
} mpart_body_processor;

mpart_body_processor* mpart_body_processor_init(http_request* request);
void mpart_body_processor_free(mpart_body_processor* p);

int mpart_body_process(http_parser *p, const char *buf, size_t len);

#endif