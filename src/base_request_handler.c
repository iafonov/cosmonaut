#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <assert.h>

#include <fcntl.h>
// #include <sys/sendfile.h> ubuntu
#include <sys/types.h>
#include <sys/uio.h>

#include "base_request_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "routing_engine.h"
#include "configuration.h"
#include "string_util.h"
#include "base_acition.h"
#include "platform.h"
#include "log.h"

extern int server_socket_fd;
extern struct global_config* configuration;

void send_response(http_response* response, int socket_fd) {
  char* serialized_headers = serialize_headers(response);

  info("sending headers:\n%s", serialized_headers);

  if (send(socket_fd, serialized_headers, strlen(serialized_headers), 0) == -1) {
    err("can not send headers");
  }

  if (response->file_path) {
    struct stat st;

    if (stat(response->file_path, &st) == 0) {
      int file_fd = open(response->file_path, O_RDONLY);

      off_t offset = 0;
      off_t len = st.st_size;

      int sent = xsendfile(socket_fd, file_fd, &offset, len);
      info("sent: %d bytes", sent);
    }
  }
}

// we are in a new fresh forked process
void handle_request(int socket_fd) {
  char request_buffer[MAX_DATA_SIZE];
  int received = 0;

  init_http_request();
  init_http_response();

  if ((received = recv(socket_fd, &request_buffer, MAX_DATA_SIZE, 0)) < 0) {
    die("something went completely wrong while receiving data");
  }

  parse_http_request(request_buffer, received);

  action matched_action = match_route(request);
  if (matched_action) {
    matched_action(request, response);
  }

  send_response(response, socket_fd);

  free_http_response();
  free_http_request();
  close(socket_fd);
}
