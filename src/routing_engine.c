#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "routing_engine.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"

extern struct global_config* configuration;

char* convert_to_local_path(char* request_path) {
  char *relative_path = malloc_str(strlen(configuration->public_root) + strlen("/") + strlen(request_path));
  sprintf(relative_path, "%s/%s", configuration->public_root, request_path);

  return relative_path;
}

void render_404(http_request* request, http_response *response) {
  response->code = 404;
  response->header_summary = "Not Found";
  response->file_path = convert_to_local_path("404.html");
  response->content_type = "text/html";
}

void render_file(http_request* request, http_response *response) {
  response->code = 200;
  response->header_summary = "OK";
  response->file_path = convert_to_local_path(request->url->path);
  response->content_type = "text/html";
}

action match_route(http_request* request) {
  char* relative_file_path = convert_to_local_path(request->url->path);
  struct stat st;

  if (stat(relative_file_path, &st) == 0 && (st.st_mode & S_IFREG)) {
    return render_file;
  }

  return render_404;
}