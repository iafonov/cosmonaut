#include "action.h"
#include "render.h"

void action_404(http_request *request, http_response *response) {
  render_file(response, "404.html");

  response->code = 404;
  response->header_summary = "Not Found";
}

void action_static_file(http_request *request, http_response *response) {
  render_file(response, request->url->path);
}
