#include <string.h>
#include <stdlib.h>

#include "str_map.h"
#include "string_util.h"
#include "log.h"

// dict's callbacks
static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static void *val_dup_cb(void *privdata, const void *src) {
  return strdup((char *)src);
}

static void *key_dup_cb(void *privdata, const void *src) {
  return strdup((char *)src);
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void val_destructor_cb(void *privdata, void *val) {
  free(val);
}

static void key_destructor_cb(void *privdata, void *key) {
  free(key);
}

static dictType str_dict = {
  hash_cb,
  key_dup_cb,
  val_dup_cb,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

str_map* str_map_init() {
  return dictCreate(&str_dict, NULL);
}

void str_map_free(str_map* h_map) {
  dictRelease(h_map);
}

char* str_map_get(str_map *h_map, const char *name) {
  dictEntry* dict_entry = dictFind(h_map, (void *)name);
  return dict_entry != NULL ? dictGetEntryVal(dict_entry) : NULL;
}

void str_map_add(str_map *h_map, char *name, char *value) {
  dictReplace(h_map, name, value);
}
