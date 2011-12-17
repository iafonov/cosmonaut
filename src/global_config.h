#ifndef _global_config_h
#define _global_config_h

struct GlobalConfig {
  char* server_port;
};

void load_configuration(char *file_name);

#endif
