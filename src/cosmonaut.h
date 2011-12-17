#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_PORT "31337"
#define SOCKET_QUEUE_SIZE 50
#define CONFIG_FILE "./config/cosmonaut.conf"

#include "./util/log.h"
#include "./util/signals.h"
#include "./util/networking.h"
#include "./util/base_request_handler.h"
#include "./util/global_config.h"