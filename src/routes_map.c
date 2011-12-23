#include <string.h>
#include <stdlib.h>

#include "routes_map.h"
#include "string_util.h"
#include "log.h"

typedef struct route_entry {
  action action_cb;
} route_entry;

void free_route_entry(route_entry* rt_entry) {
  free(rt_entry->action_cb);
  free(rt_entry);
}

// dict's callbacks
static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static void *val_dup_cb(void *privdata, const void *src) {
  route_entry *dup = malloc(sizeof(route_entry));
  memcpy(dup, src, sizeof(*dup));
  return dup;
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void key_destructor_cb(void *privdata, void *key) {
  free((char *)key);
}

static void val_destructor_cb(void *privdata, void *val) {
  free_route_entry((route_entry *)val);
}

static dictType routes_dict = {
  hash_cb,
  NULL,
  val_dup_cb,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

static char* create_normalized_path(const char *path) {
  char *result;

  if (path[0] == '/') {
    return create_str_from_str(path);
  } else {
    result = malloc_str(strlen(path) + 1);
    sprintf(result, "/%s", path);
    return result;
  }
}
// end of dict's callbacks

// public api
routes_map* routes_map_init() {
  return dictCreate(&routes_dict, NULL);
}

void routes_map_free(routes_map* rt_map) {
  dictRelease(rt_map);
}

action routes_map_get(routes_map *rt_map, const char *path) {
  char *normalized_path = create_normalized_path(path);
  action matched_action = action_404;

  dictEntry* entry = dictFind(rt_map, (void *)normalized_path);
  if (entry) {
    matched_action = ((route_entry *)entry->val)->action_cb;
  }

  free(normalized_path);
  return matched_action;
}

void routes_map_add(routes_map *rt_map, char *path, action action) {
  info("adding [%s] to route map dict", path);
  route_entry* rt_entry = malloc(sizeof(route_entry));
  rt_entry->action_cb = action;

  dictReplace(rt_map, (void *)create_str_from_str(path), (void *)rt_entry);
}
