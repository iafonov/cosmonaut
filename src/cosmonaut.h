#ifndef _cosmonaut_h
#define _cosmonaut_h

#include "http_request.h"
#include "http_response.h"

typedef void (*configure_app_cb) ();

void cosmonaut_start(int argc, char *argv[], configure_app_cb config_cb);

#endif