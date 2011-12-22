#ifndef _routing_engine_h
#define _routing_engine_h

#include "http_request.h"
#include "http_response.h"
#include "action.h"

action match_route(http_request* request);

#endif
