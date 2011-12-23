#include <stdlib.h>
#include <sys/types.h>

#include "routing_engine.h"
#include "file_util.h"
#include "string_util.h"
#include "configuration.h"
#include "routes_map.h"
#include "action.h"
#include "log.h"

extern struct global_config* configuration;

action match_route(http_request* request) {
  char* relative_file_path = create_local_path_from_request_path(request->url->path);
  action matched_action;

  if (file_exists(relative_file_path)) {
    matched_action = action_static_file;
  } else {
    matched_action = routes_map_get(configuration->routes, request->url->path);
  }

  free(relative_file_path);
  return matched_action;
}