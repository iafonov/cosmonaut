#ifndef _attrs_map_h
#define _attrs_map_h

#include "str_map.h"

typedef str_map attrs_map;

attrs_map *attrs_map_init();
void attrs_map_free(attrs_map *map);

char *attrs_map_get(attrs_map *map, const char *name);
void attrs_map_add(attrs_map *map, char *name, char *value);

void attrs_map_parse(attrs_map *map, char *header_str);

#endif
