#ifndef _render_h
#define _render_h

#include "http_response.h"

void render_file(http_response *response, const char *path);
void render_text(http_response *response, const char *text);
void render_json(http_response *response, const char *json_string);

#endif
