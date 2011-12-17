#ifndef _global_config_h
#define _global_config_h

struct GlobalConfig {
  char* server_port;
};

extern struct GlobalConfig global_config;

void load_configuration(char *file_name);

#include "global_config.c"

#endif
