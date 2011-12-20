#ifndef _base_action_h
#define _base_action_h

#include "http_request.h"
#include "http_response.h"

typedef void (*action)(http_request* request, http_response* response);

#endif
