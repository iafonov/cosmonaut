#include "cli.h"
#include "log.h"

#define DEFAULT_CONFIG_FILE "./config/cosmonaut.conf"

void parse_cli_params(struct GlobalConfig *global_config, int argc, char *argv[]) {
  // very naive but enough for now
  if (argc == 3 && strcmp(argv[1], "--config") == 0) {
    global_config->config_path = argv[2];
  } else {
    global_config->config_path = DEFAULT_CONFIG_FILE;
  }
}