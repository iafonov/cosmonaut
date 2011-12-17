#include "base_request_handler.h"

void handle_request(int new_socket_fd) {
  // we are in a new child process
  char buf[MAX_DATA_SIZE];
  int numbytes = 0;

  info("child process spawned");
  close(server_socket_fd);

  numbytes = recv(new_socket_fd, &buf, MAX_DATA_SIZE, 0);
  buf[numbytes] = '\0';
  warn("recv data:\n %s", buf);

  if (send(new_socket_fd, "It works", 8, 0) == -1) {
    err("can not send data to");
  }
  close(new_socket_fd);
  exit(0);
}
