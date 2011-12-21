#include <sys/stat.h>

#include "file_util.h"

int file_exists(char *path) {
  struct stat st;
  return stat(path, &st) == 0 && (st.st_mode & S_IFREG);
}

off_t file_size(char *path) {
  struct stat st;
  stat(path, &st);
  return st.st_size;
}
