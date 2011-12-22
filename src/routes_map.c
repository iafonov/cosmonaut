#include <string.h>
#include <stdlib.h>

#include "routes_map.h"
#include "string_util.h"
#include "log.h"

routes_map* routes_map_init() {
  return NULL;
}

void routes_map_free(routes_map* routes_map) {

}

action routes_map_get(struct routes_map *rt_map, const char *path) {
  return render_404;
}

void routes_map_add(routes_map *rt_map, char *path, action action) {

}
