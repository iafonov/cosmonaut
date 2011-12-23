#include "log.h"

struct timeval* stopwatch_time() {
  struct timeval* tv_time = malloc(sizeof(struct timeval));
  gettimeofday(tv_time, NULL);

  return tv_time;
}

void stopwatch_stop(struct timeval* start_time) {
  struct timeval* end_time = stopwatch_time();
  long msec;

  msec = (end_time->tv_sec - start_time->tv_sec) * 1000;
  msec += (end_time->tv_usec - start_time->tv_usec) / 1000;

  info("request completed in %d ms", msec);
  free(start_time);
  free(end_time);
}