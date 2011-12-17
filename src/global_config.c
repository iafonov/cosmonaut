#include "log.h"
#include "global_config.h"

extern struct GlobalConfig global_config;

void load_configuration(char *file_name) {
  global_config.server_port = "31337";
}