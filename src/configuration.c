#include "log.h"
#include "configuration.h"
#include "net.h"
#include "string_util.h"
#include "../deps/iniparser/iniparser.h"

#define DEFAULT_CONFIG_FILE "./config/cosmonaut.conf"
#define URL_NAMED_PARAM_PATTERN "\\(:([^/]*)\\)"

struct configuration *config;

static char *get_str_val(dictionary *d, char *prop_name, char *default_value) {
  char *value = iniparser_getstring(d, prop_name, default_value);

  char *config_str_ptr = malloc(strlen(value) + 1);
  strcpy(config_str_ptr, value);

  return config_str_ptr;
}

static void parse_cli_params(int argc, char *argv[]) {
  // very naive but enough for now
  if (argc == 3 && strcmp(argv[1], "--config") == 0) {
    config->config_path = argv[2];
  } else {
    config->config_path = DEFAULT_CONFIG_FILE;
  }
}

void configuration_init(int argc, char *argv[]) {
  config = malloc(sizeof(configuration));
  parse_cli_params(argc, argv);

  info("loading configuration from %s", config->config_path);
  dictionary *d = iniparser_load(config->config_path);
  char *server_hostname = net_get_hostname();

  config->server_port = get_str_val(d, "network:port", "31337");
  config->server_name = get_str_val(d, "network:server_name", server_hostname);
  config->public_root = get_str_val(d, "app:public_root", ".");
  config->uploads_root = get_str_val(d, "app:uploads_root", ".");
  config->socket_queue_size = iniparser_getint(d, "network:socket_queue_size", 50);

  config->param_match_regex = malloc(sizeof(regex_t));
  if (regcomp(config->param_match_regex, URL_NAMED_PARAM_PATTERN, REG_EXTENDED) != 0) {
    die("Invalid route parameter regex format");
  }

  config->routes = routes_map_init();

  free(server_hostname);
  iniparser_freedict(d);
}

void configuration_free() {
  free(config->server_port);
  free(config->server_name);
  free(config->public_root);
  free(config->uploads_root);

  regfree(config->param_match_regex);
  free(config->param_match_regex);
  routes_map_free(config->routes);

  free(config);
}

configuration *configuration_get() {
  return config;
}

char *configuration_convert_path_to_local(const char *request_path) {
  char *relative_path = malloc_str(strlen(configuration_get()->public_root) + strlen("/") + strlen(request_path));
  sprintf(relative_path, "%s/%s", configuration_get()->public_root, request_path);

  return relative_path;
}

route *mount(char *path, action action_cb) {
  return routes_map_add(config->routes, path, action_cb);
}
