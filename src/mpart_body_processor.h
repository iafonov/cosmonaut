#ifndef _mpart_body_processor_h
#define _mpart_body_processor_h

#include "multipart_parser.h"
#include "http_request.h"

typedef struct mpart_body_processor {
  http_request* request;
  multipart_parser* parser;
} mpart_body_processor;

mpart_body_processor* mpart_body_processor_init(http_request* request);
void mpart_body_processor_free(mpart_body_processor* p);

int mpart_body_process(http_parser *p, const char *buf, size_t len);

#endif