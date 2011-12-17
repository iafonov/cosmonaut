#include "log.h"
#include "global_config.h"
#include "../deps/iniparser/iniparser.h"

extern struct GlobalConfig global_config;

void load_configuration(char *file_name) {
  dictionary *d = iniparser_load("config/cosmonaut.conf");

  char *port = iniparser_getstring(d, "network:port", "31337");
  global_config.server_port = malloc(strlen(port) + 1);
  strcpy(global_config.server_port, port);

  global_config.socket_queue_size = iniparser_getint(d, "network:socket_queue_size", 50);

  iniparser_freedict(d);
}