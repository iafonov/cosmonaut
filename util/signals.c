#include <signal.h>
#include "log.h"

extern int server_socket;

void sigchld_handler(int signal) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void sigint_handler(int signal) {
  int yes = 1;

  info("bye");
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    die("port unbind failed")
  }
  exit(0);
}

void setup_signal_listeners() {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    exit(1);
  }

  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGINT, &sa, 0);
  sigaction(SIGTERM, &sa, 0);
}