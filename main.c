#include "server.h"
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage: %s [port]\n",argv[0]);
        exit(1);
    }
    int server_fd = server_init(atoi(argv[1]));
    while (1) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(server_fd, &set);
        select(server_fd+1, &set, NULL, NULL, NULL); 
        if (FD_ISSET(server_fd, &set)) {
            struct sockaddr_in client_addr;
            socklen_t addr_length = sizeof(client_addr);
            int client_fd  = accept(server_fd, (struct sockaddr *)&client_addr,&addr_length);
            struct client cli = {.fd = client_fd, .addr = client_addr};
            pthread_t t;
            pthread_create(&t, NULL, client_connect, (void*)&cli);
        }
    } 
    return 0;
}
