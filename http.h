#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
struct http_header {
    char method[4];
    char* ip;
    char* path;
    char* type;
    char* request;
    char* cookies;
};

struct http_header parse_http_req(char *buff, struct sockaddr_in addr);
char* http_respone_200(struct http_header req, char *data, int size);

char* http_respone_404(struct http_header req, char *data, int size);
#endif
