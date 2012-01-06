#ifndef _routes_map_h
#define _routes_map_h

#include "action.h"
#include "dict.h"

typedef dict routes_map;

routes_map* routes_map_init();
void routes_map_free(routes_map* rt_map);

action routes_map_get(routes_map *rt_map, const char *path);
void routes_map_add(routes_map *rt_map, char *path, action action);

#endif
