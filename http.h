#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct http_header {
    char method[4];
    char* path;
    char* request;
};

struct http_header parse_http_req(char *buff);
char* http_respone_200(char *data, int size);

#endif
