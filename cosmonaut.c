#include "cosmonaut.h"

struct GlobalConfig global_config;
sig_atomic_t server_socket_fd;

int main(int argc, char *argv[]) {
  int new_connection_fd;

  load_configuration(CONFIG_FILE);

  server_socket_fd = bind_server_socket_fd(global_config.server_port);
  setup_signal_listeners(server_socket_fd);

  while(1) {
    new_connection_fd = accept_connection();

    if (!fork()) {
      handle_request(new_connection_fd);
    }

    close(new_connection_fd);
  }

  return 0;
}
