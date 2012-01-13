#include "log.h"
#include "configuration.h"
#include "cli.h"
#include "networking.h"
#include "../deps/iniparser/iniparser.h"

#define URL_NAMED_PARAM_PATTERN "\\(:([^/]*)\\)"

extern struct global_config* configuration;

char* get_str_val(dictionary *d, char *prop_name, char *default_value) {
  char *value = iniparser_getstring(d, prop_name, default_value);

  char *config_str_ptr = malloc(strlen(value) + 1);
  strcpy(config_str_ptr, value);

  return config_str_ptr;
}

void load_configuration(int argc, char *argv[]) {
  configuration = malloc(sizeof(global_config));
  parse_cli_params(argc, argv);

  info("loading configuration from %s", configuration->config_path);
  dictionary *d = iniparser_load(configuration->config_path);
  char* server_hostname = determine_server_hostname();

  configuration->server_port = get_str_val(d, "network:port", "31337");
  configuration->server_name = get_str_val(d, "network:server_name", server_hostname);
  configuration->public_root = get_str_val(d, "app:public_root", ".");
  configuration->uploads_root = get_str_val(d, "app:uploads_root", ".");
  configuration->socket_queue_size = iniparser_getint(d, "network:socket_queue_size", 50);

  configuration->param_match_regex = malloc(sizeof(regex_t));
  if (regcomp(configuration->param_match_regex, URL_NAMED_PARAM_PATTERN, REG_EXTENDED) != 0) {
    die("Invalid route parameter regex format");
  }

  configuration->routes = routes_map_init();

  free(server_hostname);
  iniparser_freedict(d);
}

void free_configuration() {
  free(configuration->server_port);
  free(configuration->server_name);
  free(configuration->public_root);
  free(configuration->uploads_root);

  regfree(configuration->param_match_regex);
  routes_map_free(configuration->routes);

  free(configuration);
}

void mount(char* path, action action_cb) {
  routes_map_add(configuration->routes, path, action_cb);
}