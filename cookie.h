#ifndef COOKIE_H
#define COOKIE_H
#include <netdb.h>
typedef struct _cookie {
    char key[128];
    char value[1024];
} cookie;

struct socket_t {
    int fd;
    struct sockaddr_in addr;
};

struct socket_t init_ckserver_sk(char *ip); 
char *get_cookie(char* key);
void set_cookie(char* buff);

#endif
