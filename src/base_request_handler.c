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

// we are in a new fresh forked process
void handle_request(int socket_fd) {
  http_request* request = http_request_init(socket_fd);

  http_request_handle(request);

  http_request_free(request);
  close(socket_fd);
}
