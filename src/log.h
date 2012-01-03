// based on Zed Show debug macros
// http://c.learncodethehardway.org/book/learn-c-the-hard-waych21.html
#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>

struct timeval* stopwatch_time();
void stopwatch_stop(struct timeval* end_time);

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "[DEBUG][%d] %s:%d: " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define err(M, ...)  fprintf(stderr, "[ERROR][%d] (%s:%d) " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)
#define warn(M, ...) fprintf(stderr, "[WARN][%d] (%s:%d) " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)
#define info(M, ...) fprintf(stderr, "[INFO][%d] (%s:%d) " M "\n", getpid(), __FILE__, __LINE__, ##__VA_ARGS__)

#define die(M, ...) err(##__VA_ARGS__);exit(1);

#endif