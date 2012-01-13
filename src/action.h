#ifndef _base_action_h
#define _base_action_h

#include "http_request.h"
#include "http_response.h"

typedef void (*action)(http_request* request, http_response* response);

char* create_local_path_from_request_path(const char* request_path);

void render_file(http_response *response, const char *path);
void render_text(http_response *response, const char *text);
void render_json(http_response *response, const char *json_string);

void action_404(http_request* request, http_response *response);
void action_static_file(http_request* request, http_response *response);

#endif
