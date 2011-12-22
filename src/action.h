#ifndef _base_action_h
#define _base_action_h

#include "http_request.h"
#include "http_response.h"

typedef void (*action)(http_request* request, http_response* response);

char* create_local_path_from_request_path(char* request_path);

void render_404(http_request* request, http_response *response);
void render_file(http_request* request, http_response *response);

#endif
