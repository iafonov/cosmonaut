#ifndef _url_h
#define _url_h

typedef struct url {
  char *scheme;
  char *host;
  char *port;
  char *path;
  char *query;
  char *fragment;
  char *username;
  char *password;
} url;

url* url_init(char *raw_url);
void url_free(url* url);

#endif
