#include "/usr/local/include/cosmonaut/cosmonaut.h"

void action_index(http_request* request, http_response *response) {
  render_file(response, "index.html");
}

void action_upload(http_request* request, http_response *response) {
  render_text(response, "uploaded");
}

void configure() {
  route("/", action_index);
  route("/upload_file", action_upload);
}

int main(int argc, char *argv[]) {
  cosmonaut_start(argc, argv, configure);
}
