/*_
 * Copyright 2010 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 */

#ifndef _URL_PARSER_H
#define _URL_PARSER_H

/*
 * URL storage
 */
typedef struct parsed_url {
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
} parsed_url;

/*
 * Declaration of function prototypes
 */
struct parsed_url * parse_url(const char *);
void free_parsed_url(struct parsed_url *);


#endif /* _URL_PARSER_H */
