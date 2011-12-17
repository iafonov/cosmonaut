#ifndef _global_config_h
#define _global_config_h

struct GlobalConfig {
  char* server_port;
  int socket_queue_size;
};

void load_configuration(char *file_name);

#endif
