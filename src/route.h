#ifndef _route_h
#define _route_h

#include <regex.h>

#include "action.h"

#define MAX_PARAMS 10

typedef struct route {
  filter before_filter;
  filter after_filter;

  regex_t* matcher;
  action action;
  char* named_params[MAX_PARAMS];
  int named_params_count;
} route;

route* route_init(char *path, action action);
void route_free(route *route);
bool route_match(route* rt_entry, const char *path, params_map* params);
void route_execute_before_filter(route *route, http_request *request);

#endif _route_h