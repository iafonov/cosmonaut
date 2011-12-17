#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "./util/log.h"
#include "./util/signals.h"
#include "./util/networking.h"

#define SERVER_PORT "31337"
#define MAX_DATA_SIZE 5000

int server_socket;

int main(int argc, char *argv[]) {
  struct sockaddr their_addr;
  int new_fd;
  socklen_t addr_size = sizeof their_addr;

  setup_signal_listeners();
  server_socket = bind_server_socket(SERVER_PORT);

  char s[INET6_ADDRSTRLEN];
  char buf[MAX_DATA_SIZE];
  int numbytes = 0;

  while(1) {
    new_fd = accept(server_socket, (struct sockaddr *)&their_addr, &addr_size);

    inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    info("server: got connection from %s", s);

    if (!fork()) {
      info("child process spawned");
      close(server_socket);

      numbytes = recv(new_fd, &buf, MAX_DATA_SIZE, 0);
      buf[numbytes] = '\0';
      warn("recv data:\n %s", buf);

      if (send(new_fd, "It works", 8, 0) == -1) {
        err("can not send data to %s", s);
      }
      close(new_fd);
      exit(0);
    }

    close(new_fd);
  }

  return 0;
}
