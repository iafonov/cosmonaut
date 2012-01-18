#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

sig_atomic_t server_socket_fd;

#include "log.h"
#include "signals.h"
#include "net.h"
#include "request_handler.h"
#include "configuration.h"
#include "action.h"
#include "cosmonaut.h"

int cosmonaut_start(int argc, char *argv[], configure_app_cb config_cb) {
  int new_connection_fd;

  configuration_init(argc, argv);

  if (config_cb != NULL) {
    config_cb();
  }

  server_socket_fd = net_bind_socket();
  setup_signal_listeners(server_socket_fd);

  while(1) {
    new_connection_fd = net_accept_connection();

    if (!fork()) {
      srand(getpid());
      struct timeval *start_time = stopwatch_time();

      close(server_socket_fd);

      request_handler_handle(new_connection_fd);

      configuration_free();

      stopwatch_stop(start_time);
      exit(0);
    }

    close(new_connection_fd);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  return cosmonaut_start(argc, argv, NULL);
}
