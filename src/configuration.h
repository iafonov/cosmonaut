#ifndef _configuration_h
#define _configuration_h

#include "routes_map.h"

typedef struct global_config {
  char* config_path;
  char* server_port;
  char* server_name;

  char* public_root;
  int socket_queue_size;

  routes_map* routes;
} global_config;

void load_configuration(int argc, char *argv[]);
void free_configuration();
void route(char* path, action action_cb);

#endif
