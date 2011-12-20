#include <unistd.h>
#include <signal.h>

struct GlobalConfig* configuration;
sig_atomic_t server_socket_fd;

#include "log.h"
#include "signals.h"
#include "networking.h"
#include "base_request_handler.h"
#include "configuration.h"

int main(int argc, char *argv[]) {
  int new_connection_fd;

  load_configuration(argc, argv);

  server_socket_fd = bind_server_socket_fd();
  setup_signal_listeners(server_socket_fd);

  while(1) {
    new_connection_fd = accept_connection();

    if (!fork()) {
      handle_request(new_connection_fd);
      free_configuration();
      exit(0);
    }

    close(new_connection_fd);
  }

  free_configuration();
  return 0;
}
