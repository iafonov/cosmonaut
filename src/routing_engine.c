#include <stdlib.h>
#include <sys/types.h>

#include "routing_engine.h"
#include "file_util.h"
#include "string_util.h"
#include "configuration.h"
#include "routes_map.h"
#include "action.h"
#include "log.h"

action routing_engine_match(http_request* request) {
  char* relative_file_path = configuration_convert_path_to_local(request->url->path);
  action matched_action;

  if (file_exists(relative_file_path)) {
    matched_action = action_static_file;
  } else {
    matched_action = routes_map_process_path(configuration_get()->routes, request->url->path, request->params);
  }

  free(relative_file_path);
  return matched_action;
}