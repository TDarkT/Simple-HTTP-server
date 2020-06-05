#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
struct client {
    int fd;
    struct sockaddr_in addr;
};
int server_init();
void *client_connect(void *arg);
#endif
