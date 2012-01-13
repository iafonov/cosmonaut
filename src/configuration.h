#ifndef _configuration_h
#define _configuration_h

#include <regex.h>

#include "routes_map.h"

typedef struct global_config {
  char* config_path;
  char* server_port;
  char* server_name;

  char* public_root;
  char* uploads_root;
  int socket_queue_size;

  regex_t* param_match_regex;
  routes_map* routes;
} global_config;

void load_configuration(int argc, char *argv[]);
void free_configuration();
void mount(char* path, action action_cb);

#endif
