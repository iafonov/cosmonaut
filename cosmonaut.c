#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_PORT "31337"
#define SOCKET_QUEUE_SIZE 50

#include "./util/log.h"
#include "./util/signals.h"
#include "./util/networking.h"
#include "./util/base_request_handler.h"

sig_atomic_t server_socket_fd;

int main(int argc, char *argv[]) {
  server_socket_fd = bind_server_socket_fd(SERVER_PORT);
  setup_signal_listeners(server_socket_fd);
  int new_connection_fd;

  while(1) {
    new_connection_fd = accept_connection();

    if (!fork()) {
      handle_request(new_connection_fd);
    }

    close(new_connection_fd);
  }

  return 0;
}
