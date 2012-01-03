#ifndef _headers_map_h
#define _headers_map_h

#include "str_map.h"

typedef str_map headers_map;

headers_map* headers_map_init();
void headers_map_free(headers_map* h_map);

char* headers_map_get(headers_map *h_map, const char *name);
void headers_map_add(headers_map *h_map, char *name, char *value);

char* headers_map_serialize(headers_map *h_map, char* http_header);

#endif
