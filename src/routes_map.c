#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "routes_map.h"
#include "string_util.h"
#include "configuration.h"
#include "route.h"
#include "log.h"
#include "action.h"

static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void key_destructor_cb(void *privdata, void *key) {
  free(key);
}

static void val_destructor_cb(void *privdata, void *val) {
  route_free((route *) val);
}

static dictType routes_dict = {
  hash_cb,
  NULL,
  NULL,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

static char* create_normalized_path(const char *path) {
  char *result;

  if (path[0] == '/') {
    return strdup(path);
  } else {
    result = malloc_str(strlen(path) + 1);
    sprintf(result, "/%s", path);
    return result;
  }
}

routes_map* routes_map_init() {
  return dictCreate(&routes_dict, NULL);
}

void routes_map_free(routes_map* rt_map) {
  dictRelease(rt_map);
}

route* routes_map_match(routes_map *rt_map, const char *path, params_map* params) {
  char *normalized_path = create_normalized_path(path);

  dictIterator* dict_iterator;
  dictEntry* dict_entry;
  route *result = NULL;

  dict_iterator = dictGetIterator(rt_map);
  while((dict_entry = dictNext(dict_iterator)) != NULL) {
    route *route = dictGetEntryVal(dict_entry);

    if (route_match(route, normalized_path, params)) {
      result = route;
      break;
    }
  }

  free(dict_iterator);
  free(normalized_path);
  return result;
}

route* routes_map_add(routes_map *rt_map, char *path, action action) {
  route* route = route_init(path, action);

  dictAdd(rt_map, (void *)strdup(path), (void *)route);

  return route;
}
