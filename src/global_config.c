#include "log.h"
#include "global_config.h"
#include "cli.h"
#include "networking.h"
#include "../deps/iniparser/iniparser.h"

char* get_str_val(dictionary *d, char *prop_name, char *default_value) {
  char *value = iniparser_getstring(d, prop_name, default_value);

  char *config_str_ptr = malloc(strlen(value) + 1);
  strcpy(config_str_ptr, value);

  return config_str_ptr;
}

void load_configuration(struct GlobalConfig *global_config, int argc, char *argv[]) {
  parse_cli_params(global_config, argc, argv);

  info("loading config from %s", global_config->config_path);
  dictionary *d = iniparser_load(global_config->config_path);

  global_config->server_port = get_str_val(d, "network:port", "31337");
  global_config->server_name = get_str_val(d, "network:server_name", determine_server_hostname());
  global_config->public_root = get_str_val(d, "app:public_root", ".");
  global_config->socket_queue_size = iniparser_getint(d, "network:socket_queue_size", 50);

  iniparser_freedict(d);
}