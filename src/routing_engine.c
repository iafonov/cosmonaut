#include "routing_engine.h"
#include "log.h"

void blank_action(http_request* request, http_response *response) {
  info("it fucking works!");
}

action match_route(http_request* request) {
  return blank_action;
}