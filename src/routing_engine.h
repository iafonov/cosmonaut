#ifndef _routing_engine_h
#define _routing_engine_h

#include "http_request.h"
#include "http_response.h"

void routing_engine_execute_action(http_request *request, http_response *response);

#endif
