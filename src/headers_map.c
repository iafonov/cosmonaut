#include <string.h>
#include <stdlib.h>

#include "headers_map.h"
#include "string_util.h"
#include "log.h"

headers_map *headers_map_init() {
  return str_map_init();
}

void headers_map_free(headers_map *h_map) {
  str_map_free(h_map);
}

char *headers_map_get(headers_map *h_map, const char *name) {
  return str_map_get(h_map, name);
}

void headers_map_add(headers_map *h_map, char *name, char *value) {
  str_map_add(h_map, name, value);
}

char *headers_map_serialize(headers_map *h_map, char *http_header) {
  dictIterator *dict_iterator;
  dictEntry *dict_entry;
  char *key, *val, *header_line;

  dict_iterator = dictGetIterator(h_map);

  while((dict_entry = dictNext(dict_iterator)) != NULL) {
    key = dictGetEntryKey(dict_entry);
    val = dictGetEntryVal(dict_entry);

    header_line = malloc_str(strlen(key) + strlen(": ") + strlen(val) + strlen("\n"));
    sprintf(header_line, "%s: %s\n", key, val);

    http_header = realloc(http_header, 1 + strlen(http_header) + strlen(header_line));
    http_header = strncat(http_header, header_line, strlen(header_line));

    free(header_line);
  }

  http_header = realloc(http_header, strlen(http_header) + strlen("\n") + 1);
  http_header = strncat(http_header, "\n", strlen("\n"));

  free(dict_iterator);
  return http_header;
}
