#include "/usr/local/include/cosmonaut/cosmonaut.h"

void do_work() {
  sleep(1);
}

void action_index(http_request* request, http_response *response) {
  do_work();
  render_file(response, "index.html");
}

void action_upload(http_request* request, http_response *response) {
  do_work();
  render_text(response, "uploaded");
}

void configure() {
  route("/", action_index);
  route("/upload_file", action_upload);
}

int main(int argc, char *argv[]) {
  cosmonaut_start(argc, argv, configure);
}
