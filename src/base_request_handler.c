#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "base_request_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "routing_engine.h"
#include "configuration.h"
#include "string_util.h"
#include "action.h"
#include "platform.h"
#include "log.h"

extern int server_socket_fd;

void send_response(http_response* response, int socket_fd) {
  char* serialized_headers = http_response_serialize_headers(response);

  if (send(socket_fd, serialized_headers, strlen(serialized_headers), 0) == -1) {
    err("can not send headers");
  }

  free(serialized_headers);

  if (response->file_path) {
    struct stat st;

    if (stat(response->file_path, &st) == 0) {
      int file_fd = open(response->file_path, O_RDONLY);

      off_t offset = 0;
      off_t len = st.st_size;

      xsendfile(socket_fd, file_fd, &offset, len);
    }
  } else if (response->raw_response) {
    if (send(socket_fd, response->raw_response, strlen(response->raw_response), 0) == -1) {
      err("can not send headers");
    }
  }
}

// we are in a new fresh forked process
void handle_request(int socket_fd) {
  http_request* request = http_request_init();
  http_response* response = http_response_init();

  http_request_parse(request, socket_fd);

  action matched_action = match_route(request);
  if (matched_action) {
    matched_action(request, response);
  }

  send_response(response, socket_fd);

  http_response_free(response);
  http_request_free(request);
  close(socket_fd);
}
