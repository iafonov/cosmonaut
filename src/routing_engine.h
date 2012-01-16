#ifndef _routing_engine_h
#define _routing_engine_h

#include "http_request.h"
#include "http_response.h"
#include "action.h"

action routing_engine_match(http_request* request);

#endif
