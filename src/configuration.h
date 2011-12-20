#ifndef _configuration_h
#define _configuration_h

typedef struct global_config {
  char* config_path;
  char* server_port;
  char* server_name;

  char* public_root;
  int socket_queue_size;
} global_config;

void load_configuration(int argc, char *argv[]);
void free_configuration();

#endif
