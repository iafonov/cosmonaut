#ifndef _platform_h
#define _platform_h

#include <sys/types.h>

int xsendfile(int out_fd, int in_fd, off_t *offset, size_t count);

#endif
