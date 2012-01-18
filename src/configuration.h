#ifndef _configuration_h
#define _configuration_h

#include <regex.h>

#include "action.h"
#include "routes_map.h"

typedef struct configuration {
  char *config_path;
  char *server_port;
  char *server_name;

  char *public_root;
  char *uploads_root;
  int socket_queue_size;

  regex_t *param_match_regex;
  routes_map *routes;
} configuration;

void configuration_init(int argc, char *argv[]);
void configuration_free();
configuration *configuration_get();

char *configuration_convert_path_to_local(const char *request_path);

route *mount(char *path, action action_cb);

#endif
