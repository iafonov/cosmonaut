#include "log.h"
#include "global_config.h"
#include "../deps/iniparser/iniparser.h"

extern struct GlobalConfig global_config;

char* get_str_val(dictionary *d, char *prop_name, char *default_value) {
  char *value = iniparser_getstring(d, prop_name, default_value);

  char *config_str_ptr = malloc(strlen(value) + 1);
  strcpy(config_str_ptr, value);

  return config_str_ptr;
}

void load_configuration(char *file_name) {
  dictionary *d = iniparser_load("config/cosmonaut.conf");

  global_config.server_port = get_str_val(d, "network:port", "31337");
  global_config.socket_queue_size = iniparser_getint(d, "network:socket_queue_size", 50);

  iniparser_freedict(d);
}