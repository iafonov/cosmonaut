#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

struct global_config* configuration;
sig_atomic_t server_socket_fd;

#include "log.h"
#include "signals.h"
#include "networking.h"
#include "base_request_handler.h"
#include "configuration.h"
#include "action.h"
#include "cosmonaut.h"

void action_index(http_request* request, http_response *response) {
  render_file(response, "index.html");
}

void action_upload(http_request* request, http_response *response) {
  render_text(response, params_map_get(request->params, "file")->val);
}

void configure() {
  route("/", action_index);
  route("/upload_file", action_upload);
}

void cosmonaut_start(int argc, char *argv[], configure_app_cb config_cb) {
  int new_connection_fd;

  load_configuration(argc, argv);

  config_cb();

  server_socket_fd = bind_server_socket_fd();
  setup_signal_listeners(server_socket_fd);

  while(1) {
    new_connection_fd = accept_connection();

    if (!fork()) {
      struct timeval* start_time = stopwatch_time();

      close(server_socket_fd);

      handle_request(new_connection_fd);

      free_configuration();

      stopwatch_stop(start_time);
      exit(0);
    }

    close(new_connection_fd);
  }
}

int main(int argc, char *argv[]) {
  cosmonaut_start(argc, argv, configure);

  return 0;
}
