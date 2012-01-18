#ifndef _str_map_h
#define _str_map_h

#include "dict.h"

typedef dict str_map;

str_map *str_map_init();
void str_map_free(str_map *map);

char *str_map_get(str_map *map, const char *name);
void str_map_add(str_map *map, char *name, char *value);

#endif
