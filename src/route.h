#ifndef _route_h
#define _route_h

#include <regex.h>

#include "action.h"

#define MAX_PARAMS 10

typedef struct route {
  action action;
  regex_t* matcher;

  char* named_params[MAX_PARAMS];
  int named_params_count;
} route;

route* route_init(char *path, action action);
route* route_copy(const route *src);
bool route_match(route* rt_entry, const char *path, params_map* params);
void configuration_free(route *route);

#endif _route_h