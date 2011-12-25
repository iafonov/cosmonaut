#include <string.h>
#include <stdlib.h>

#include "headers_map.h"
#include "string_util.h"
#include "log.h"

// dict's callbacks
static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static void *val_dup_cb(void *privdata, const void *src) {
  char *dup = malloc_str(strlen((char *)src));
  strncpy(dup, src, strlen((char *)src));
  return dup;
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void val_destructor_cb(void *privdata, void *val) {
  free(val);
}

static dictType headers_dict = {
  hash_cb,
  NULL,
  val_dup_cb,
  key_comapre_cb,
  NULL,
  val_destructor_cb
};
// end of dict's callbacks

headers_map* headers_map_init() {
  return dictCreate(&headers_dict, NULL);
}

void headers_map_free(headers_map* h_map) {
  dictRelease(h_map);
}

char* headers_map_get(headers_map *h_map, const char *name) {
  dictEntry* dict_entry = dictFind(h_map, (void *)name);
  return dict_entry != NULL ? dictGetEntryVal(dict_entry) : NULL;
}

void headers_map_add(headers_map *h_map, char *name, char *value) {
  dictReplace(h_map, name, value);
}

char* headers_map_serialize(headers_map *h_map, char* http_header) {
  dictIterator* dict_iterator;
  dictEntry* dict_entry;
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
