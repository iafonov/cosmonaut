#include <stdlib.h>
#include <sys/types.h>

#include "routing_engine.h"
#include "file_util.h"
#include "string_util.h"
#include "configuration.h"
#include "routes_map.h"
#include "action.h"
#include "log.h"

void routing_engine_execute_action(http_request *request, http_response *response) {
  char* relative_file_path = configuration_convert_path_to_local(request->url->path);

  if (file_exists(relative_file_path)) {
    action_static_file(request, response);
  } else if (request->route) {
    request->route->action(request, response);
  } else {
    action_404(request, response);
  }

  free(relative_file_path);

}