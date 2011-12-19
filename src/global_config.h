#ifndef _global_config_h
#define _global_config_h

struct GlobalConfig {
  char* config_path;
  char* server_port;
  char* public_root;
  int socket_queue_size;
};

void load_configuration(struct GlobalConfig *global_config, int argc, char *argv[]);

#endif
