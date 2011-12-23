#include "platform.h"
#include "log.h"

#ifdef LINUX
  #include <sys/sendfile.h>
#elif DARWIN
  #include <sys/socket.h>
  #include <sys/uio.h>
#endif

int xsendfile(int out_fd, int in_fd, off_t *offset, size_t count) {
#ifdef LINUX
  return sendfile(out_fd, in_fd, offset, count);
#elif DARWIN
  // int sendfile(int fd, int s, off_t offset, off_t *len, struct sf_hdtr *hdtr, int flags);
  off_t len = count;

  sendfile(in_fd, out_fd, *offset, &len, (void *)0, 0);
  return len;
#else
  return 0;
#endif
}
