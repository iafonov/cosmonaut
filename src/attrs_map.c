#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "attrs_map.h"
#include "string_util.h"
#include "log.h"

attrs_map* attrs_map_init() {
  return str_map_init();
}

void attrs_map_free(attrs_map* map) {
  str_map_free(map);
}

char* attrs_map_get(attrs_map *map, const char *name) {
  return str_map_get(map, name);
}

void attrs_map_add(attrs_map *map, char *name, char *value) {
  str_map_add(map, name, value);
}

void attrs_map_parse(attrs_map *map, char* str) {
  char *pair, *name, *value, *header_str, *original_ptr;
  header_str = strdup(str);
  original_ptr = header_str;

  while(isspace(*header_str)) header_str++;

  while((pair = strsep(&header_str, ";")) && pair != NULL) {
    name = strsep(&pair, "=");
    value = strsep(&pair, "=");

    attrs_map_add(map, str_trim(name), str_trim(value));
  }

  free(original_ptr);
}
