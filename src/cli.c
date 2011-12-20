#include "cli.h"
#include "log.h"

#define DEFAULT_CONFIG_FILE "./config/cosmonaut.conf"

extern struct GlobalConfig* configuration;

void parse_cli_params(int argc, char *argv[]) {
  // very naive but enough for now
  if (argc == 3 && strcmp(argv[1], "--config") == 0) {
    configuration->config_path = argv[2];
  } else {
    configuration->config_path = DEFAULT_CONFIG_FILE;
  }
}