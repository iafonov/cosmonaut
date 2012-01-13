#include "/usr/local/include/cosmonaut/cosmonaut.h"

void do_work() {
  sleep(1);
}

void action_index(http_request* request, http_response *response) {
  render_file(response, "index.html");
}

void action_upload(http_request* request, http_response *response) {
  do_work(); // time out - to force cucumber to wait before checking file presense
  render_text(response, "uploaded");
}

void action_echo(http_request* request, http_response *response) {
  char* serialized_params = params_map_serialize(request->params);
  render_text(response, serialized_params);
  free(serialized_params);
}

void configure() {
  mount("/", action_index);
  mount("/upload_file", action_upload);

  mount("/photos/(:id)", action_echo);
  mount("/patients/(:id)/(:action)", action_echo);
  mount("/gallery/(:gallery_id)/photo/(:photo_id)", action_echo);
}

int main(int argc, char *argv[]) {
  cosmonaut_start(argc, argv, configure);
}
